#include "Editor.h"

#include "Escape.h"
#include "Settings.h"
#include "SpecialKey.h"

//
// bool Editor::CheckCharacter(char c) {
//    int currentX = editorState_.GetXOffset() + cursorX_;
//    int currentY = editorState_.GetYOffset() + cursorY_;
//
//    if (currentY < 0 || currentY >= editorState_.GetNumRowsWithData()) return false;
//    if (currentX < 0 || currentX >= editorState_.GetLineWidth(cursorY_)) return false;
//
//    return editorState_.GetWholeLine(cursorY_)[currentX] == c;
// }

void Editor::HideCursor() {
   if (cursorHidden_) {
      throw std::logic_error("HideCursor() called when cursor hidden!");
   }
   cursorHidden_ = true;
   Draw(escape::HideCursor());
}

void Editor::ShowCursor() {
   if (!cursorHidden_) {
      throw std::logic_error("ShowCursor() called when cursor not hidden!");
   }
   cursorHidden_ = false;
   Draw(escape::ShowCursor());
}

// This writes the escape characters to the buffer - NOT to the terminal
// This writes the escape characters to the buffer - NOT to the terminal
void Editor::ClearLineRightOfCursor() {
   Draw(escape::ClearLineRightOfCursor());
}

void Editor::ClearScreen() {
   Draw(escape::ClearScreen());
}

void Editor::JumpToFirstPixel() {
   // Write an escape sequence that jumps to the beginning of the terminal
   Draw(escape::JumpToFirstPixel());
}

void Editor::UpdateCursorPosition(int x, int y) {
   Draw(escape::SetCursorPosition(x, y));
}

void Editor::UpdateRenderCursor(const std::string_view &line) {
   renderX_ = 0;

   if (!DataExistsAtY(cursorY_)) return;
   if (cursorX_ < 0) renderX_ = cursorX_;

   for (int j = 0; j < cursorX_; j++) {
      if (line[j] == '\t') {
         renderX_ += (settings::TAB_WIDTH - 1) - (renderX_ % settings::TAB_WIDTH);
      }
      renderX_++;
   }
}

void Editor::SetHelpMessage(const std::string_view &text) {
   helpMessage_ = text;
   helpMessageStartTime_ = std::chrono::system_clock::now();
}

void Editor::DrawHelpMessage() {
   Draw("\r\n");
   Draw(escape::ClearLineRightOfCursor());

   if (!helpMessage_.has_value()) {
      Draw(std::string(editorState_.GetScreenWidth() - 1, ' '));
      return;
   }

   std::string helpMessage = std::format("HELP: {}", helpMessage_.value());

   if (std::chrono::system_clock::now() - helpMessageStartTime_ < std::chrono::seconds(5)) {
      Draw(escape::SetTextColourToGreen());
      Draw(helpMessage_.value());
      Draw(escape::DefaultTextFormatting());
   }

}

void Editor::DrawStatusBar() {
   Draw(escape::EnableInvertedColours());

   // Other code prohibits drawing a newline on the last line
   Draw("\r\n");

   if (!editorState_.GetFileName().has_value()) {
      Draw(std::string(editorState_.GetScreenWidth() - 1, ' '));
   } else {
      std::string fileName = editorState_.GetFileName().value();
      int currentLine = editorState_.GetYOffset() + cursorY_ + 1;

      std::string informationString = std::format("{} Line:{}", fileName, currentLine);


      Draw(std::format("{}{:^{}}",informationString, " ",
            editorState_.GetScreenWidth() - informationString.size())
      );
   }

   Draw(escape::DefaultTextFormatting());
}

void Editor::DrawSplashScreen() {
   for (int y = 0; y < editorState_.GetScreenHeight(); y++) {

      if (y == editorState_.GetScreenHeight() / 3) {
         std::string welcomeMessage = std::format("Tiny editor -- version {}", TINY_EDITOR_VERSION);
         Draw(std::format("{:^{}}", welcomeMessage, editorState_.GetScreenWidth()));
      } else {
         Draw("->");
      }

      Draw(escape::ClearLineRightOfCursor());
      if (y != editorState_.GetScreenHeight() - 1) {
         Draw("\r\n");
      }
   }
}

bool Editor::CursorAtEndOfLine() {
   int currentX = editorState_.GetXOffset() + cursorX_;
   int currentY = editorState_.GetYOffset() + cursorY_;

   // If we're in a weird state
   if (currentY < 0 || currentY >= editorState_.GetNumLinesWithData()) return false;

   if (currentX == editorState_.GetLineWidth(cursorY_)) return true;
   return false;
}

bool Editor::DataExistsAtY(int row) {
   return editorState_.GetYOffset() + row >= 0
       && editorState_.GetYOffset() + row < editorState_.GetNumLinesWithData();
}

std::string_view Editor::GetVisibleRenderCharactersAtRow(int i) {
   std::string_view fullRow = editorState_.GetRenderLine(i);
   int colOffset = editorState_.GetXOffset();

   if (colOffset >= fullRow.length()) {
      return "";
   }

   std::string_view offsetRow = fullRow.substr(colOffset);
   return offsetRow.substr(0, std::min(editorState_.GetScreenWidth(), static_cast<int>(offsetRow.length())));
}

std::string_view Editor::GetVisibleFileCharactersAtRow(int i) {
   std::string_view fullRow = editorState_.GetFileLine(i);
   int colOffset = editorState_.GetXOffset();

   if (colOffset >= fullRow.length()) {
      return "";
   }

   std::string_view offsetRow = fullRow.substr(colOffset);
   return offsetRow.substr(0, std::min(editorState_.GetScreenWidth(), static_cast<int>(offsetRow.length())));
}

void Editor::Draw(std::string_view text) {
   appendBuffer_.Append(text);
}

void Editor::DrawRows() {
   if (editorState_.GetNumLinesWithData() == 0) {
      DrawSplashScreen();
   } else {
      for (int y = 0; y < editorState_.GetScreenHeight(); y++) {
         Draw(escape::ClearLineRightOfCursor());

         if (DataExistsAtY(y)) {
            Draw(GetVisibleRenderCharactersAtRow(y));
         } else {
            Draw("->");
         }

         if (y != editorState_.GetScreenHeight() - 1) {
            Draw("\r\n");
         }
      }
   }

   DrawStatusBar();
   DrawHelpMessage();
}

void Editor::RefreshScreen() {
   HideCursor();
   {
      JumpToFirstPixel();
      DrawRows();
      UpdateCursorPosition(renderX_, cursorY_);
   }
   ShowCursor();

   appendBuffer_.WriteBufferToSTDOUT();
   appendBuffer_.ClearBuffer();
}

int Editor::ReadKey() {
   char c = '\0';
   std::size_t numberOfBitsRead = read(STDIN_FILENO, &c, 1);

   if (numberOfBitsRead == -1 && errno != EAGAIN) { // EAGAIN is an error CYGWIN terminals report for empty result
      utils::FailAndExit("read() failed when trying to read from stdin!");
   }

   return c;
}

void Editor::MoveCursor(int c) {
   switch (c) {
      case ARROW_UP:    --cursorY_;  break;
      case ARROW_DOWN:  ++cursorY_;  break;

      case ARROW_LEFT: {
         if (cursorX_ > 0) {
            --cursorX_;
         } else if (cursorY_ > 0) {
            --cursorY_;
            cursorX_ = editorState_.GetLineWidth(cursorY_) - 1;
         }
         break;
      }
      case ARROW_RIGHT: {
         if (!CursorAtEndOfLine()) {
            ++cursorX_;
         } else if (DataExistsAtY(cursorY_ + 1)) {
            ++cursorY_;
            cursorX_ = 0;
         }
         break;
      }

      case PAGE_UP: {
         if (cursorY_ == 0) {
            cursorY_ = -editorState_.GetScreenHeight();
         } else {
            cursorY_ = 0;
         }
         break;
      }
      case PAGE_DOWN: {
         if (cursorY_ == editorState_.GetScreenHeight() - 1) {
            cursorY_ += editorState_.GetScreenHeight();
         } else {
            cursorY_ = editorState_.GetScreenHeight()-1;
         }
         break;
      }

      case HOME: {
         if (cursorX_ == 0) {
            cursorX_ = -editorState_.GetScreenWidth();
         } else {
            cursorX_ = 0;
         }
         break;
      }
      case END: {
         if (cursorX_ == editorState_.GetScreenWidth() - 1) {
            cursorX_ += editorState_.GetScreenWidth();
         }else {
            cursorX_ = editorState_.GetScreenWidth()-1;
         }
         break;
      }
   }

   Scroll();
}

void Editor::Scroll() {
   UpdateRenderCursor(GetVisibleFileCharactersAtRow(cursorY_));

   int lineLength = 0;
   int currentY = editorState_.GetYOffset() + cursorY_;

   if (currentY >= 0 && currentY < editorState_.GetNumLinesWithData()) {
      lineLength = editorState_.GetLineWidth(cursorY_);
   }

   // Clamp the cursor to the last character of the row
   int currentX = editorState_.GetXOffset() + renderX_;
   if (currentX >= lineLength) {
      renderX_ -= currentX;                         // move it back to zero
      renderX_ += std::max(0, lineLength - 1); // then move it to the end of the line
   }
   // Clamp the cursor to the last line in the file
   if (currentY >= editorState_.GetNumLinesWithData()) {
      cursorY_ -= (1 + currentY - editorState_.GetNumLinesWithData());
   }

   // Handle Horizontal Scrolling
   if (renderX_ < 0) {
      editorState_.AddToXOffsetIfPossible(renderX_);
      renderX_ = 0;
   }
   else if (renderX_ >= editorState_.GetScreenWidth()) {
      editorState_.AddToXOffsetIfPossible(renderX_ - editorState_.GetScreenWidth() + 1);
      renderX_ = editorState_.GetScreenWidth() - 1;
   }

   // Handle Vertical Scrolling
   if (cursorY_ < 0) {
      editorState_.AddToYOffsetIfPossible(cursorY_);
      cursorY_ = 0;
   }
   else if (cursorY_ >= editorState_.GetScreenHeight()) {
      editorState_.AddToYOffsetIfPossible(cursorY_ - editorState_.GetScreenHeight() + 1);
      cursorY_ = editorState_.GetScreenHeight() - 1;
   }
}

// Returns bool for if we should exit.
void Editor::ProcessKeypress(int c) {
   if (c == ESCAPE_CHAR) {
      c = ConvertEscapeKey();
   }

   switch (c) {
      case ARROW_UP:
      case ARROW_LEFT:
      case ARROW_DOWN:
      case ARROW_RIGHT:
      case PAGE_UP:
      case PAGE_DOWN:
      case HOME:
      case END:                      MoveCursor(c);           break;
      case utils::ControlKey('q'): shouldContinue_ = false; break;
   }
}

SpecialKey Editor::ConvertEscapeKey() {
   char seq[3];
   // The user might have just pressed the ESCAPE key!
   if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESCAPE_CHAR;
   if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESCAPE_CHAR;

   // NOTE: Different terminals + OSes use different keys for HOME & END
   if (seq[0] == '[') {

      // 3 character escape codes where the second character is a digit
      if ('0' <= seq[1] && seq[1] <= '9') {
         if (read(STDIN_FILENO, &seq[2], 1) != 1) return ESCAPE_CHAR;

         if (seq[2] == '~') {
            switch (seq[1]) {
               case '1': return HOME;
               case '3': return DELETE;
               case '4': return END;
               case '5': return PAGE_UP;
               case '6': return PAGE_DOWN;
               case '7': return HOME;
               case '8': return END;
            }
         }
      }
      // 2 character escape codes
      else {
         switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;

            case 'F':  return END;
            case 'H':  return HOME;
         }
      }
   }
   else if (seq[0] == 'O') {
      switch (seq[1]) {
         case 'H': return HOME;
         case 'F': return END;
      }
   }

   return ESCAPE_CHAR;
}
