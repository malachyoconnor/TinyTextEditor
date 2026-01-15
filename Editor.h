#ifndef TINYTEXTEDITOR_EDITOR_H
#define TINYTEXTEDITOR_EDITOR_H
#include "AppendBuffer.h"
#include "EditorState.h"
#include "SpecialKey.h"

constexpr std::string TINY_EDITOR_VERSION = "0.0.1";

class Editor {
public:
   Editor(EditorState& editorState)
      : shouldContinue_(true),
        cursorHidden_(false),
        editorState_(editorState),
        appendBuffer_(AppendBuffer()),
        cursorX_(0),
        cursorY_(0),
        renderX_(0),
        numEditorRows_(0),
        numCharsModified_(0),
        helpMessage_(std::nullopt),
        helpMessageStartTime_() {
   }

   void             SaveFile();

   bool             CursorAtEndOfLine();
   bool             DataExistsAtY(int row);

   void             Draw(std::string_view text);
   void             DrawRows();
   void             MoveCursor(int c);
   void             ProcessKeypress(int ch);
   void             RefreshScreen();
   void             Scroll();
   std::string_view GetVisibleFileCharactersAtRow(int i);
   std::string_view GetVisibleRenderCharactersAtRow(int i);
   int              ReadKey();
   SpecialKey       ConvertEscapeKey();

   bool             InsertChar(int toInsert);

   void             ClearScreen();
   void             JumpToFirstPixel();
   void             UpdateCursorPosition(int x, int y);
   void             UpdateRenderCursor(const std::string_view &line);
   void             SetHelpMessage(const std::string_view &text);
   void             DrawHelpMessage();
   void             DrawStatusBar();
   void             DrawSplashScreen();

   void             HideCursor();
   void             ShowCursor();
   void             ClearLineRightOfCursor();

   [[nodiscard]]
   bool ShouldContinue() const { return shouldContinue_; }


private:
   bool                       shouldContinue_;
   bool                       cursorHidden_;
   EditorState&               editorState_;
   AppendBuffer               appendBuffer_;
   int                        cursorX_;
   int                        cursorY_;
   int                        renderX_;
   std::string                editorRow_;
   int                        numEditorRows_;
   int                        numCharsModified_;
   std::optional<std::string> helpMessage_;
   std::chrono::system_clock::time_point helpMessageStartTime_;
};


#endif //TINYTEXTEDITOR_EDITOR_H