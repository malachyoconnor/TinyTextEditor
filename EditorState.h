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
      columnOffset_(0),
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
   void AddToColumnOffsetIfPossible(int n);

   [[nodiscard]]
   const std::string_view GetWholeRow() const {
      return std::string_view(allRows_[rowOffset_]);
   }

   [[nodiscard]]
   const std::string_view GetWholeRow(int i) const {
      return std::string_view(allRows_[i + rowOffset_]);
   }

   int GetRowOffset() const { return rowOffset_; }
   int GetColumnOffset() const { return columnOffset_; }
   int GetCurrentRowWidth() const { return GetWholeRow().length(); }

   int GetScreenRows() const { return screenRows_; }
   int GetScreenCols() const { return screenCols_; }
   int GetNumRowsWithData() const { return numRowsWithData_; }

private:
   int screenRows_;
   int screenCols_;
   std::optional<termios> initialTerminalAttributes_;

   int rowOffset_;
   int columnOffset_;
   std::vector<std::string> allRows_;
   int numRowsWithData_;

   AppendBuffer buf_;
};

#endif //TINYTEXTEDITOR_EDITORSTATE_H