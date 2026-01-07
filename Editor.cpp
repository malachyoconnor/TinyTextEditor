#include "Editor.h"

void Editor::DrawRows() {
   for (int y = 0; y < editorState_.GetScreenCols(); y++) {
      write(STDOUT_FILENO, "->", 2);
      if (y != editorState_.GetScreenCols() - 1) {
         write(STDOUT_FILENO, "\r\n", 2);
      }
   }
   terminal::JumpToFirstPixel();
}

void Editor::RefreshScreen() {
   terminal::ClearScreen();
   terminal::JumpToFirstPixel();
   DrawRows();
}

char Editor::ReadKey() {
   char c = '\0';
   std::size_t numberOfBitsRead = read(STDIN_FILENO, &c, 1);

   if (numberOfBitsRead == -1 && errno != EAGAIN) { // EAGAIN is an error CYGWIN terminals report for empty result
      utils::FailAndExit("read() failed when trying to read from stdin!");
   }

   return c;
}

// Returns bool for if we should exit.
void Editor::ProcessKeypress(char c) {
   switch (c) {
      case utils::ControlKey('q'): { shouldContinue_ = false; break; }
   }
}