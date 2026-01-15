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
   EditorState() : screenY_(-1), screenX_(-1), initialTerminalAttributes_(std::nullopt),
      yOffset_(0), xOffset_(0), currentFilePath_(std::nullopt), fileBuffer_({}) {
      EnableRawMode();

      if (utils::isDebuggerAttached()) {
         screenX_ = 0; screenY_ = 0;
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
   ~EditorState();

   void EnableRawMode();
   void UpdateRenderBuffer(const std::string &row, int index);
   void AppendLine(const std::string& line);
   void InsertCharAt(int y, int x, char c);
   void OpenFile(const std::filesystem::path &path);

   void AddToYOffsetIfPossible(int n);
   void AddToXOffsetIfPossible(int n);

   std::string_view GetRenderLine() const;
   std::string_view GetRenderLine(int i) const;
   std::string_view GetFileLine() const;
   std::string_view GetFileLine(int i) const;
   std::optional<std::filesystem::path> GetFilePath() const;

   int GetYOffset() const { return yOffset_; }
   int GetXOffset() const { return xOffset_; }

   int GetLineWidth(int i) const { return GetRenderLine(i).length(); }
   int GetNumLinesWithData() const { return renderBuffer_.size(); }

   int GetScreenHeight() const { return screenY_; }
   int GetScreenWidth() const { return screenX_; }

   std::optional<std::string> GetFileName() const;

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