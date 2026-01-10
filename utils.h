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

      // std::exit(1);
      throw std::runtime_error(msg);
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

   // TODO: Make this a real logger

   const std::string LOG_FILE_PATH = ROOT_DIRECTORY "/LOG.txt";
   static bool logFileOpened = false;
   static std::ofstream logFile {};

   inline void LOG(std::string_view msg) {
      if (logFileOpened) {
         logFile << "LOG: " << msg << std::endl;
         return;
      }

      logFileOpened = true;
      // Note: This clears any currently existing log files.
      logFile.open(LOG_FILE_PATH, std::ofstream::out | std::ofstream::trunc);
      if (!logFile.is_open()) {
         FailAndExit(std::format("Failed to open log file: {}", LOG_FILE_PATH));
      }

      logFile << "LOG: " << msg << std::endl;
   }

}

#endif //TINYTEXTEDITOR_UTILS_H