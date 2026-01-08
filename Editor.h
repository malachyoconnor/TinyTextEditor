#ifndef TINYTEXTEDITOR_EDITOR_H
#define TINYTEXTEDITOR_EDITOR_H
#include "EditorState.h"
#include "KeyPress.h"

constexpr std::string TINY_EDITOR_VERSION = "0.0.1";

class Editor {
public:
   Editor(EditorState& editorState)
   : shouldContinue_(true),
     editorState_(editorState),
     appendBuffer_(AppendBuffer()),
     cursorHidden_(false),
   cursorX_(0),
   cursorY_(0)
   {}

   void DrawRows();
   void RefreshScreen();
   int ReadKey();
   void ProcessKeypress(int c);
   SpecialKey ConvertEscapeKey();

   void ClearScreen();
   void JumpToFirstPixel();
   void UpdateCursorPosition(int x, int y);
   void HideCursor();
   void ShowCursor();
   void ClearLineRightOfCursor();

   [[nodiscard]]
   bool ShouldContinue() const { return shouldContinue_; }


private:
   bool shouldContinue_;
   EditorState& editorState_;
   AppendBuffer appendBuffer_;
   bool cursorHidden_;
   int cursorX_;
   int cursorY_;
};


#endif //TINYTEXTEDITOR_EDITOR_H