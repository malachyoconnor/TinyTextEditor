#include "Editor.h"

#include "Escape.h"
#include "KeyPress.h"

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

void Editor::DrawRows() {
   for (int y = 0; y < editorState_.GetScreenRows(); y++) {

      if (y == editorState_.GetScreenRows() / 3) {
         std::string welcomeMessage = std::format("Tiny editor -- version {}", TINY_EDITOR_VERSION);

         appendBuffer_.Append(std::format("{:^{}}", welcomeMessage, editorState_.GetScreenCols()));
      } else {
         appendBuffer_.Append("->");
      }


      appendBuffer_.Append(escape::ClearLineRightOfCursor());
      if (y != editorState_.GetScreenRows() - 1) {
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

// Returns bool for if we should exit.
void Editor::ProcessKeypress(int c) {
   if (c == ESCAPE_CHAR) {
      c = ConvertEscapeKey();
   }

   switch (c) {
      case ARROW_UP:                 cursorY_ = std::max(cursorY_-1, 0);                              break;
      case ARROW_LEFT:               cursorX_ = std::max(cursorX_-1, 0);                              break;
      case ARROW_DOWN:               cursorY_ = std::min(cursorY_+1, editorState_.GetScreenRows()-1); break;
      case ARROW_RIGHT:              cursorX_ = std::min(cursorX_+1, editorState_.GetScreenCols()-1); break;

      case PAGE_UP:                  cursorY_ = 0;                                                        break;
      case PAGE_DOWN:                cursorY_ = editorState_.GetScreenRows()-1;                           break;
      case HOME:                     cursorX_ = 0;                                                        break;
      case END:                      cursorX_ = editorState_.GetScreenCols()-1;                           break;


      case utils::ControlKey('q'): shouldContinue_ = false;                                             break;
   }
}

SpecialKey Editor::ConvertEscapeKey() {
   char seq[3];
   // The user might have just pressed the ESCAPE key!
   if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESCAPE_CHAR;
   if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESCAPE_CHAR;

   if (seq[0] == '[') {

      // 3 character escape codes where the second character is a digit
      if ('0' <= seq[1] && seq[1] <= '9') {
         if (read(STDIN_FILENO, &seq[2], 1) != 1) return ESCAPE_CHAR;

         if (seq[2] == '~') {
            switch (seq[1]) {
               case '5': return PAGE_UP;
               case '6': return PAGE_DOWN;
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

   return ESCAPE_CHAR;
}
