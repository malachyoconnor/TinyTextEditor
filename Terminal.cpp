#include "Terminal.h"

#include <unistd.h>
#include "utils.h"
#include <sys/ioctl.h>

namespace terminal {
   void JumpToFirstPixel() {
      // Write an escape sequence that jumps to the beginning of the terminal
      write(STDOUT_FILENO, "\x1b[H", 3);
   }

   void JumpToLastPixel() {
      // First we move 999 "pixels" right, then 999 "pixels" down
      if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
         utils::FailAndExit("Error writing to terminal when trying to jump to last pixel!");
      }
   }

   void ClearScreen() {
      // Write an escape sequence that clears the screen
      write(STDOUT_FILENO, "\x1b[2J", 4);
   }

   std::pair<int,int> GetCursorPosition() {
      // This asks for the cursor position from the terminal
      if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
         utils::FailAndExit("Couldn't get cursor position from terminal!");
      }

      char buf[32];
      uint i = 0;

      while (i < sizeof(buf) - 1) {
         if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
         if (buf[i] == 'R') break;
         i++;
      }
      buf[i] = '\0';

      int rows, cols;
      if (buf[0] != '\x1b' || buf[1] != '[') {
         utils::FailAndExit("Cursor position response invalid");
      }
      if (sscanf(&buf[2], "%d;%d", &rows, &cols) != 2) {
         utils::FailAndExit("Buffer invalid when getting cursor");
      }

      return {rows, cols};
   }

   std::pair<int, int> GetWindowSize() {
      winsize windowSize {};
      bool ioctlFailed = ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize) == -1;

      if (ioctlFailed || windowSize.ws_col == 0) {
         // TODO: This has the side-effect of jumping to the very first pixel when we get the window size
         JumpToLastPixel();
         return GetCursorPosition();
      }
      return {windowSize.ws_col, windowSize.ws_row};
   }
}