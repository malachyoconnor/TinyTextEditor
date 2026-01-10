#include "Editor.h"

#include "Escape.h"
#include "SpecialKey.h"

void Editor::HideCursor() {
   if (cursorHidden_) {
      throw std::logic_error("HideCursor() called when cursor hidden!");
   }
   cursorHidden_ = true;
   appendBuffer_.Append(escape::HideCursor());
}

void Editor::ShowCursor() {
   if (!cursorHidden_) {
      throw std::logic_error("ShowCursor() called when cursor not hidden!");
   }
   cursorHidden_ = false;
   appendBuffer_.Append(escape::ShowCursor());
}

// This writes the escape characters to the buffer - NOT to the terminal
// This writes the escape characters to the buffer - NOT to the terminal
void Editor::ClearLineRightOfCursor() {
   appendBuffer_.Append(escape::ClearLineRightOfCursor());
}

void Editor::ClearScreen() {
   appendBuffer_.Append(escape::ClearScreen());
}

void Editor::JumpToFirstPixel() {
   // Write an escape sequence that jumps to the beginning of the terminal
   appendBuffer_.Append(escape::JumpToFirstPixel());
}

void Editor::UpdateCursorPosition(int x, int y) {
   appendBuffer_.Append(escape::SetCursorPosition(x, y));
}

void Editor::DrawSplashScreen() {
   for (int y = 0; y < editorState_.GetScreenHeight(); y++) {

      if (y == editorState_.GetScreenHeight() / 3) {
         std::string welcomeMessage = std::format("Tiny editor -- version {}", TINY_EDITOR_VERSION);
         appendBuffer_.Append(std::format("{:^{}}", welcomeMessage, editorState_.GetScreenWidth()));
      } else {
         appendBuffer_.Append("->");
      }

      appendBuffer_.Append(escape::ClearLineRightOfCursor());
      if (y != editorState_.GetScreenHeight() - 1) {
         appendBuffer_.Append("\r\n");
      }
   }
}

bool Editor::DataExistsAtRow(int row) {
   return editorState_.GetYOffset() + row >= 0
       && editorState_.GetYOffset() + row < editorState_.GetNumRowsWithData();
}

std::string_view Editor::GetVisibleCharactersAtRow(int i) {
   std::string_view fullRow = editorState_.GetWholeLine(i);
   int colOffset = editorState_.GetXOffset();

   if (colOffset >= fullRow.length()) {
      return "";
   }

   std::string_view offsetRow = fullRow.substr(colOffset);
   return offsetRow.substr(0, std::min(editorState_.GetScreenWidth(), static_cast<int>(offsetRow.length())));
}

void Editor::DrawRows() {
   if (editorState_.GetNumRowsWithData() == 0) { DrawSplashScreen(); return; }

   for (int y = 0; y < editorState_.GetScreenHeight(); y++) {
      appendBuffer_.Append(escape::ClearLineRightOfCursor());

      if (DataExistsAtRow(y)) {
         appendBuffer_.Append(GetVisibleCharactersAtRow(y));
      } else {
         appendBuffer_.Append("->");
      }

      if (y != editorState_.GetScreenHeight() - 1) {
         appendBuffer_.Append("\r\n");
      }
   }
}

void Editor::RefreshScreen() {
   HideCursor();
   {
      JumpToFirstPixel();
      DrawRows();
      UpdateCursorPosition(cursorX_, cursorY_);
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
      case ARROW_UP:                 --cursorY_;                                    break;
      case ARROW_LEFT:               --cursorX_;                                    break;
      case ARROW_DOWN:               ++cursorY_;                                    break;
      case ARROW_RIGHT:              ++cursorX_;                                    break;

      // TODO: This implementation is more useful for scrolling apparently
      // {
      //    int times = E.screenrows;
      //    while (times--)
      //       editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      // }
      case PAGE_UP:                  cursorY_ = 0;                                  break;
      case PAGE_DOWN:                cursorY_ = editorState_.GetScreenHeight()-1;     break;
      case HOME:                     cursorX_ = 0;                                  break;
      case END:                      cursorX_ = editorState_.GetScreenWidth()-1;     break;
   }

   EditorScroll();
}

void Editor::EditorScroll() {
   int lineLength = 0;
   int currentY = editorState_.GetYOffset() + cursorY_;

   if (currentY >= 0 && currentY < editorState_.GetNumRowsWithData()) {
      lineLength = static_cast<int>(editorState_.GetWholeLine(cursorY_).length());
   }

   // Clamp the cursor to the last character of the row
   int currentX = editorState_.GetXOffset() + cursorX_;
   if (currentX >= lineLength) {
      int adjustment = std::max(0, lineLength - 1) - currentX;
      cursorX_ += adjustment;
   }

   // Handle Horizontal Scrolling
   if (cursorX_ < 0) {
      editorState_.AddToXOffsetIfPossible(cursorX_);
      cursorX_ = 0;
   }
   else if (cursorX_ >= editorState_.GetScreenWidth()) {
      editorState_.AddToXOffsetIfPossible(cursorX_ - editorState_.GetScreenWidth() + 1);
      cursorX_ = editorState_.GetScreenWidth() - 1;
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
