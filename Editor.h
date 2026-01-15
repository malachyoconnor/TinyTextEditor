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
     renderX_(0)
     {}

   bool             CursorAtEndOfLine();
   bool             DataExistsAtY(int row);

   std::string_view GetVisibleRenderCharactersAtRow(int i);
   std::string_view GetVisibleFileCharactersAtRow(int i);
   void             Draw(std::string_view text);
   void             DrawRows();
   void             RefreshScreen();
   int              ReadKey();
   void             MoveCursor(int c);
   void             Scroll();
   void             ProcessKeypress(int c);
   SpecialKey       ConvertEscapeKey();

   void             ClearScreen();
   void             JumpToFirstPixel();
   void             UpdateCursorPosition(int x, int y);
   void             UpdateRenderCursor(const std::string_view &line);
   void             DrawSplashScreen();

   void             HideCursor();
   void             ShowCursor();
   void             ClearLineRightOfCursor();

   [[nodiscard]]
   bool ShouldContinue() const { return shouldContinue_; }


private:
   bool         shouldContinue_;
   bool         cursorHidden_;
   EditorState& editorState_;
   AppendBuffer appendBuffer_;
   int          cursorX_;
   int          cursorY_;
   int          renderX_;
   std::string  editorRow_;
   int          numEditorRows_;
};


#endif //TINYTEXTEDITOR_EDITOR_H