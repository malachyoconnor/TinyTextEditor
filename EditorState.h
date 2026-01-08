#ifndef TINYTEXTEDITOR_EDITORSTATE_H
#define TINYTEXTEDITOR_EDITORSTATE_H
#include <format>
#include <optional>
#include <termios.h>
#include <filesystem>
#include <vector>

#include "AppendBuffer.h"
#include "Terminal.h"
#include "utils.h"

class EditorState {
public:
   EditorState() :
      screenRows_(-1),
      screenCols_(-1),
      initialTerminalAttributes_(std::nullopt),
      rowOffset_(0),
      allRows_({}),
      numRowsWithData_(0),
      buf_(AppendBuffer())
   {
      EnableRawMode();

      if (utils::isDebuggerAttached()) {
         screenCols_ = 0;
         screenRows_ = 0;
         return;
      }

      std::tie(screenCols_, screenRows_) = terminal::GetWindowSize();
      if (screenRows_ == -1 || screenCols_ == -1) {
         utils::FailAndExit(std::format("Read rows and columns as ({},{})", screenRows_, screenCols_));
      }
   }

   void EnableRawMode();
   // TEMP
   void AppendRow(const std::string& row);
   void OpenFile(const std::filesystem::path &path);
   ~EditorState();

   void AddToRowOffsetIfPossible(int n);

   [[nodiscard]]
   const std::string& GetRow(int i) const {
      return allRows_[i + rowOffset_];
   }

   [[nodiscard]]
   int GetRowOffset() const {
      return rowOffset_;
   }

   [[nodiscard]]
   int GetScreenRows() const { return screenRows_; }

   [[nodiscard]]
   int GetScreenCols() const { return screenCols_; }

   [[nodiscard]]
   int GetNumRowsWithData() const { return numRowsWithData_; }

private:
   int screenRows_;
   int screenCols_;
   std::optional<termios> initialTerminalAttributes_;

   int rowOffset_;
   std::vector<std::string> allRows_;
   int numRowsWithData_;

   AppendBuffer buf_;
};

#endif //TINYTEXTEDITOR_EDITORSTATE_H