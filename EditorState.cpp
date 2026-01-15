#include "EditorState.h"

#include <iostream>
#include <ostream>

#include "Settings.h"

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

void EditorState::AddToYOffsetIfPossible(int n) {
   yOffset_ += n;

   if (yOffset_ < 0) {
      yOffset_ = 0;
   }
   else if (yOffset_ >= renderBuffer_.size()) {
      yOffset_ = renderBuffer_.size() - 1;
   }
}

void EditorState::AddToXOffsetIfPossible(int n) {
   xOffset_ += n;

   if (xOffset_ < 0) {
      xOffset_ = 0;
   }
}

std::string_view EditorState::GetRenderLine() const {
   return std::string_view(renderBuffer_[yOffset_]);
}

std::string_view EditorState::GetRenderLine(int i) const {
   return std::string_view(renderBuffer_[i + yOffset_]);
}

std::string_view EditorState::GetFileLine() const {
   return std::string_view(fileBuffer_[yOffset_]);
}

std::string_view EditorState::GetFileLine(int i) const {
   return std::string_view(fileBuffer_[i + yOffset_]);
}

std::optional<std::filesystem::path> EditorState::GetFilePath() const {
   return currentFilePath_;
}

std::optional<std::string> EditorState::GetFileName() const {
   if (currentFilePath_.has_value()) {
      return currentFilePath_.value().filename();
   }
   return std::nullopt;
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

void EditorState::UpdateRenderBuffer(const std::string &row, int index) {
   std::string result {};

   for (char c : row) {
      if (c == '\t') {
         result += std::string(settings::TAB_WIDTH, ' ');
      } else {
        result += c;
      }
   }
   if (index >= renderBuffer_.size()) {
      renderBuffer_.push_back(result);
   } else {
      renderBuffer_[index] = result;
   }
}

void EditorState::AppendLine(const std::string &line) {
   fileBuffer_.push_back(line);
   UpdateRenderBuffer(line, fileBuffer_.size());
}

void EditorState::InsertCharAt(int y, int x, char c) {
   const std::string& originalLine = fileBuffer_[y + yOffset_];

   fileBuffer_[y + yOffset_] = originalLine.substr(0, x + xOffset_) + c + originalLine.substr(x + xOffset_);
   UpdateRenderBuffer(fileBuffer_[y + yOffset_], y + yOffset_);
}

void EditorState::OpenFile(const std::filesystem::path& path) {
   std::ifstream fileStream (path);
   if (!fileStream.is_open()) {
      utils::FailAndExit(std::format("Could not open file: {}", path.string()));
   }

   currentFilePath_.emplace(path);

   std::string line;
   while (std::getline(fileStream, line)) {
      AppendLine(line);
   }

   if (!fileStream.eof()) {
      utils::FailAndExit(std::format("Could not read the whole file: {}", path.string()));
   }
   if (fileStream.bad()) {
      utils::FailAndExit(std::format("Could not read file: {}", path.string()));
   }

}
