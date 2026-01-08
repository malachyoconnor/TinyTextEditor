#ifndef TINYTEXTEDITOR_UTILS_H
#define TINYTEXTEDITOR_UTILS_H
#include <fstream>
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

   // This prevents debugging if we're using CLION terminal
   inline bool isDebuggerAttached() {
      // We don't want to read the status file over and over
      static int pid = -1;
      if (pid != -1) return pid != 0;
      std::ifstream statusFile("/proc/self/status");
      std::string line;
      while (std::getline(statusFile, line)) {
         if (line.find("TracerPid:") == 0) {
            pid = std::stoi(line.substr(10));
            return pid != 0;
         }
      }
      return false;
   }

}

#endif //TINYTEXTEDITOR_UTILS_H