#ifndef TINYTEXTEDITOR_EDITORSTATE_H
#define TINYTEXTEDITOR_EDITORSTATE_H
#include <format>
#include <optional>
#include <termios.h>
#include "AppendBuffer.h"
#include "Terminal.h"
#include "utils.h"

class EditorState {
public:
   EditorState() {
      auto [numRows, numCols] = terminal::GetWindowSize();
      if (numRows == -1 || numCols == -1) {
         utils::FailAndExit(std::format("Read rows and columns as ({},{})", numRows, numCols));
      }

      screenCols_ = numCols;
      screenRows_ = numRows;
   }

   ~EditorState();

   [[nodiscard]]
   int GetScreenRows() const { return screenRows_; }

   [[nodiscard]]
   int GetScreenCols() const { return screenCols_; }

   void EnableRawMode();

private:
   int screenRows_ = -1;
   int screenCols_ = -1;
   std::optional<termios> initialTerminalAttributes_ = std::nullopt;

   AppendBuffer buf;
};

#endif //TINYTEXTEDITOR_EDITORSTATE_H