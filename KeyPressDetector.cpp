// THIS IS A HELPER SCRIPT
#include <iostream>
#include "utils.h"

#include "EditorState.h"

int main() {
   EditorState state {};

   std::cout << "Press Ctrl + Q to quit \r" << std::endl;

   while (true) {

      char readChar;
      const ssize_t bitsRead = read(STDIN_FILENO, &readChar, 1);

      if (readChar == utils::ControlKey('q')) return 0;
      if (bitsRead < 1) continue;

      std::cout << std::format("{} ({})", static_cast<int>(readChar), readChar) << '\r' << std::endl;
   }
}
