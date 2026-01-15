#ifndef TINYTEXTEDITOR_EDITORSTATE_H
#define TINYTEXTEDITOR_EDITORSTATE_H
#include <format>
#include <optional>
#include <termios.h>
#include <filesystem>
#include <vector>

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
      currentFilePath_(std::nullopt),
      fileBuffer_({})
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

      // Add space for a status bar at the bottom
      screenY_ -= 1;
      // Add space for a message bar
      screenY_ -= 1;
   }

   void EnableRawMode();
   // TEMP
   void UpdateRenderBuffer(const std::string &row);
   void AppendLine(const std::string& row);
   void OpenFile(const std::filesystem::path &path);
   ~EditorState();

   void AddToYOffsetIfPossible(int n);
   void AddToXOffsetIfPossible(int n);
   void AddToRenderXOffsetIfPossible(int n);

   [[nodiscard]]
   const std::string_view GetRenderLine() const {
      return std::string_view(renderBuffer_[yOffset_]);
   }

   [[nodiscard]]
   const std::string_view GetRenderLine(int i) const {
      return std::string_view(renderBuffer_[i + yOffset_]);
   }

   [[nodiscard]]
  const std::string_view GetFileLine() const {
      return std::string_view(fileBuffer_[yOffset_]);
   }

   [[nodiscard]]
   const std::string_view GetFileLine(int i) const {
      return std::string_view(fileBuffer_[i + yOffset_]);
   }

   int GetYOffset() const { return yOffset_; }
   int GetXOffset() const { return xOffset_; }

   int GetCurrentLineWidth() const { return GetRenderLine().length(); }
   int GetLineWidth(int i) const { return GetRenderLine(i).length(); }

   int GetScreenHeight() const { return screenY_; }
   int GetScreenWidth() const { return screenX_; }
   int GetNumLinesWithData() const { return renderBuffer_.size(); }

   std::optional<std::string> GetFileName() const {
      if (currentFilePath_.has_value()) {
         return currentFilePath_.value().filename();
      }
      return std::nullopt;
   }

private:
   int screenY_;
   int screenX_;
   std::optional<termios> initialTerminalAttributes_;

   int yOffset_;
   int xOffset_;

   std::optional<std::filesystem::path> currentFilePath_;

   // Two buffers needed - one for the raw file and one for what gets rendered.
   std::vector<std::string> fileBuffer_;
   std::vector<std::string> renderBuffer_;
};

#endif //TINYTEXTEDITOR_EDITORSTATE_H