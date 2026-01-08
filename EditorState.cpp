#include "EditorState.h"

#include <iostream>
#include <ostream>

EditorState::~EditorState() {
   if (!initialTerminalAttributes_.has_value()) {
      utils::FailAndExit("We're trying to disable RAW mode, but the initial terminal attributes has no value!");
   }
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &initialTerminalAttributes_.value()) == -1) {
      utils::FailAndExit("tcsetattr() when trying to disable raw mode");
   }

   terminal::ClearScreen();
   write(STDOUT_FILENO, "Goodbye!", sizeof("Goodbye!"));
}

void EditorState::AddToRowOffsetIfPossible(int n) {
   rowOffset_ += n;

   if (rowOffset_ < 0) {
      rowOffset_ = 0;
   }
   else if (rowOffset_ >= allRows_.size()) {
      rowOffset_ = allRows_.size() - 1;
   }
}

void EditorState::AddToColumnOffsetIfPossible(int n) {
   columnOffset_ += n;

   if (columnOffset_ < 0) {
      columnOffset_ = 0;
   }
}

void EditorState::EnableRawMode() {
   if (initialTerminalAttributes_.has_value()) {
      throw std::logic_error("Tried to update initial terminal attributes more than once!");
   }

   // Get the attributes struct for the terminal and store into the initial struct
   initialTerminalAttributes_.emplace(termios{});
   if (tcgetattr(STDIN_FILENO, &initialTerminalAttributes_.value()) == -1) {
      utils::FailAndExit("tcgetattr() failed when trying to update initial terminal attributes");
   }
   termios terminalAttributes = initialTerminalAttributes_.value();

   terminalAttributes.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
   // ICRNL:  Disables Ctrl+M - normally would output a carriage return
   // IXON:   Disables Ctrl+S and Ctrl-Q from pausing and resuming transmission to our program
   // BRKINT: Disables a break condition causing a SIGINT signal
   // INPCK:  Disables parity checking, which doesn’t seem to apply to modern terminal emulators.
   // ISTRIP: Disables the 8th bit of each input byte being set to 0
   terminalAttributes.c_oflag &= ~(OPOST);
   // OPOST:  Disables converting \n -> \r\n
   terminalAttributes.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
   // ECHO:   Disables the terminal echoing whatever we type
   // ICANON: Disables the terminal outputting line by line, now it's byte by byte!
   // ISIG:   Disables Ctrl+C & Ctrl+Z from exiting our program
   // IEXTEN: Disables Ctrl+V - makes some terminals convert the next character to the literal byte. I.e. Ctrl-C -> 3
   terminalAttributes.c_cflag |= (CS8);
   // CS8:    Sets the character size (CS) to 8 bits per byte.

   // This sets the minimum number of bytes that can be read before read returns to zero
   terminalAttributes.c_cc[VMIN] = 0;
   // This sets the read timeout time to 1 * (100 milliseconds)
   terminalAttributes.c_cc[VTIME] = 1;
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminalAttributes);
}

void EditorState::AppendRow(const std::string &row) {
   allRows_.push_back(row);
   ++numRowsWithData_;
}

void EditorState::OpenFile(const std::filesystem::path& path) {
   std::ifstream fileStream (path);
   if (!fileStream.is_open()) {
      utils::FailAndExit(std::format("Could not open file: {}", path.string()));
   }

   std::string line;
   while (std::getline(fileStream, line)) {
      AppendRow(line);
   }

   if (!fileStream.eof()) {
      utils::FailAndExit(std::format("Could not read the whole file: {}", path.string()));
   }
   if (fileStream.bad()) {
      utils::FailAndExit(std::format("Could not read file: {}", path.string()));
   }


}
