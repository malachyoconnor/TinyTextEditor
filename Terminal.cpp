#include "Terminal.h"

#include <unistd.h>
#include "utils.h"
#include <sys/ioctl.h>

#include "Escape.h"

namespace terminal {

   ssize_t Write(const std::string& str) {
      return write(STDOUT_FILENO, str.c_str(), str.size());
   }

   // Returns if everything was written
   bool WriteAll(const std::string& str) {
      return write(STDOUT_FILENO, str.c_str(), str.size()) == str.size();
   }

   void JumpToFirstPixel() {
      // Write an escape sequence that jumps to the beginning of the terminal
      Write(escape::JumpToFirstPixel());
   }

   void JumpToLastPixel() {
      if (!WriteAll(escape::MoveRight(999) + escape::MoveDown(999))) {
         utils::FailAndExit("Error writing to terminal when trying to jump to last pixel!");
      }
   }

   void ClearScreen() {
      Write(escape::ClearScreen());
   }

   std::pair<int,int> GetCursorPosition() {
      // CLION debugging
      if (utils::isDebuggerAttached()) return {0,0};

      // This asks for the cursor position from the terminal
      if (!WriteAll(escape::RequestCursorPosition())) {
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

      return {cols, rows};
   }

   // Returns {num columns, num rows}
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
