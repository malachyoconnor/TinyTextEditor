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
      screenY_(-1),
      screenX_(-1),
      initialTerminalAttributes_(std::nullopt),
      yOffset_(0),
      xOffset_(0),
      allLines_({}),
      buf_(AppendBuffer())
   {
      EnableRawMode();

      if (utils::isDebuggerAttached()) {
         screenX_ = 0;
         screenY_ = 0;
         return;
      }

      std::tie(screenX_, screenY_) = terminal::GetWindowSize();
      if (screenY_ == -1 || screenX_ == -1) {
         utils::FailAndExit(std::format("Read rows and columns as ({},{})", screenY_, screenX_));
      }
   }

   void EnableRawMode();
   // TEMP
   void AppendLine(const std::string& row);
   void OpenFile(const std::filesystem::path &path);
   ~EditorState();

   void AddToYOffsetIfPossible(int n);
   void AddToXOffsetIfPossible(int n);

   [[nodiscard]]
   const std::string_view GetWholeLine() const {
      return std::string_view(allLines_[yOffset_]);
   }

   [[nodiscard]]
   const std::string_view GetWholeLine(int i) const {
      return std::string_view(allLines_[i + yOffset_]);
   }

   int GetYOffset() const { return yOffset_; }
   int GetXOffset() const { return xOffset_; }
   int GetCurrentLineWidth() const { return GetWholeLine().length(); }

   int GetScreenHeight() const { return screenY_; }
   int GetScreenWidth() const { return screenX_; }
   int GetNumRowsWithData() const { return allLines_.size(); }

private:
   int screenY_;
   int screenX_;
   std::optional<termios> initialTerminalAttributes_;

   int yOffset_;
   int xOffset_;
   std::vector<std::string> allLines_;

   AppendBuffer buf_;
};

#endif //TINYTEXTEDITOR_EDITORSTATE_H