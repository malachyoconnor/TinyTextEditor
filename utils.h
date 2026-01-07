#ifndef TINYTEXTEDITOR_UTILS_H
#define TINYTEXTEDITOR_UTILS_H
#include <string>
#include <unistd.h>

namespace utils {

   consteval char ControlKey(char k) {
      return static_cast<char>(k & 0x1f);
   }

   inline void FailAndExit(const std::string& msg) {
      // Clear the screen
      write(STDOUT_FILENO, "\x1b[2J", 4);
      // First prints our message. Then checks the errno bit and prints the error found there.
      perror(msg.c_str());
      std::exit(1);
   }
}

#endif //TINYTEXTEDITOR_UTILS_H