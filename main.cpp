#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "AppendBuffer.h"

void EditorClearScreen();
void EditorJumpToFirstPixel();

struct EditorState {
   int screenRows = -1;
   int screenCols = -1;
   std::optional<termios> initialTerminalAttributes = std::nullopt;

   AppendBuffer buf;
};
EditorState EDITOR_STATE{};

consteval char ControlKey(char k) {
   return static_cast<char>(k & 0x1f);
}

//*** LOW LEVEL ***//

void FailAndExit(const std::string& msg) {
   EditorClearScreen();
   // First prints our message. Then checks the errno bit and prints the error found there.
   perror(msg.c_str());
   std::exit(1);
}

void EnableRawMode() {
   if (EDITOR_STATE.initialTerminalAttributes.has_value()) {
      throw std::logic_error("Tried to update initial terminal attributes more than once!");
   }

   // Get the attributes struct for the terminal and store into the initial struct
   EDITOR_STATE.initialTerminalAttributes.emplace(termios{});
   if (tcgetattr(STDIN_FILENO, &EDITOR_STATE.initialTerminalAttributes.value()) == -1) {
      FailAndExit("tcgetattr() failed when trying to update initial terminal attributes");
   }
   termios terminalAttributes = EDITOR_STATE.initialTerminalAttributes.value();

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

void DisableRawMode() {
   if (!EDITOR_STATE.initialTerminalAttributes.has_value()) {
      FailAndExit("We're trying to disable RAW mode, but the initial terminal attributes has no value!");
   }
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &EDITOR_STATE.initialTerminalAttributes.value()) == -1) {
      FailAndExit("tcsetattr() when trying to disable raw mode");
   }

   EditorClearScreen();
   std::cout << "GoodBye!" << std::endl;
}

// *** TERMINAL *** //
void EditorJumpToFirstPixel() {
   // Write an escape sequence that jumps to the beginning of the terminal
   write(STDOUT_FILENO, "\x1b[H", 3);
}

void EditorJumpToLastPixel() {
   // First we move 999 "pixels" right, then 999 "pixels" down
   if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
      FailAndExit("Error writing to terminal when trying to jump to last pixel!");
   }
}

void EditorClearScreen() {
   // Write an escape sequence that clears the screen
   write(STDOUT_FILENO, "\x1b[2J", 4);
}

std::pair<int,int> getCursorPosition() {
   // This asks for the cursor position from the terminal
   if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
      FailAndExit("Couldn't get cursor position from terminal!");
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
   if (buf[0] != '\x1b' || buf[1] != '[')                   FailAndExit("Cursor position response invalid");
   if (sscanf(&buf[2], "%d;%d", &rows, &cols) != 2) FailAndExit("Buffer invalid when getting cursor");

   return {rows, cols};
}

std::pair<int, int> getWindowSize() {
   winsize windowSize {};
   bool ioctlFailed = ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize) == -1;

   if (ioctlFailed || windowSize.ws_col == 0) {
      EditorJumpToLastPixel();
      return getCursorPosition();
   }
   return {windowSize.ws_col, windowSize.ws_row};
}


void EditorDrawRows() {
   for (int y = 0; y < EDITOR_STATE.screenCols; y++) {
      write(STDOUT_FILENO, "->", 2);
      if (y != EDITOR_STATE.screenCols - 1) {
         write(STDOUT_FILENO, "\r\n", 2);
      }
   }
   EditorJumpToFirstPixel();
}

void EditorRefreshScreen() {
   EditorClearScreen();
   EditorJumpToFirstPixel();
   EditorDrawRows();
}

char EditorReadKey() {
   char c = '\0';
   std::size_t numberOfBitsRead = read(STDIN_FILENO, &c, 1);

   if (numberOfBitsRead == -1 && errno != EAGAIN) { // EAGAIN is an error CYGWIN terminals report for empty result
      FailAndExit("read() failed when trying to read from stdin!");
   }

   return c;
}

//*** PROCESSING ***//

void EditorProcessKeypress(char c) {
   switch (c) {
      case ControlKey('q'): {
         std::exit(0);
      }
   }
}

//*** INIT ***//

void InitEditor() {
   auto [numRows, numCols] = getWindowSize();
   if (numRows == -1 || numCols == -1) {
      FailAndExit(std::format("Read rows and columns as ({},{})", numRows, numCols));
   }

   EDITOR_STATE.screenCols = numCols;
   EDITOR_STATE.screenRows = numRows;
}


int main() {
   EnableRawMode();
   std::atexit(DisableRawMode);
   InitEditor();

   while (true) {
      char c = EditorReadKey();

      EditorRefreshScreen();
      EditorProcessKeypress(c);
   }

}