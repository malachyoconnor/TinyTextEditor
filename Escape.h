#ifndef TINYTEXTEDITOR_ESCAPE_H
#define TINYTEXTEDITOR_ESCAPE_H
#include <format>
#include <string>

namespace escape {

   constexpr std::string ClearLineRightOfCursor() {
      return "\x1b[0K";
   }

   constexpr std::string ClearWholeLine() {
      return "\x1b[K2";
   }

   constexpr std::string ClearScreen() {
      return "\x1b[2J";
   }

   constexpr std::string JumpToFirstPixel() {
      // Write an escape sequence that jumps to the beginning of the terminal
      return "\x1b[H";
   }

   constexpr std::string HideCursor() {
      return "\x1b[?25l";
   }

   constexpr std::string ShowCursor() {
      return "\x1b[?25h";
   }

   constexpr std::string SetCursorPosition(int x, int y) {
      // Terminals are 1 indexed!
      return std::format("\x1b[{};{}H", y+1, x+1);
   }

   constexpr std::string MoveRight(int numPixelsToMove) {
      return std::format("\x1b[{}C", numPixelsToMove);
   }

   constexpr std::string MoveDown(int numPixelsToMove) {
      return std::format("\x1b[{}B", numPixelsToMove);
   }

   constexpr std::string RequestCursorPosition() {
      return "\x1b[6n";
   }

   constexpr std::string EnableInvertedColours() {
      return std::format("\x1b[7m");
   }

   constexpr std::string SetTextColourToGreen() {
      return "\x1b[32m";
   }

   constexpr std::string DefaultTextFormatting() {
      return "\x1b[m";
   }

}

#endif //TINYTEXTEDITOR_ESCAPE_H