#ifndef TINYTEXTEDITOR_TERMINAL_H
#define TINYTEXTEDITOR_TERMINAL_H
#include <utility>


namespace terminal {
   void JumpToFirstPixel();
   void JumpToLastPixel();
   void ClearScreen();
   std::pair<int,int> GetCursorPosition();
   std::pair<int, int> GetWindowSize();
}


#endif //TINYTEXTEDITOR_TERMINAL_H