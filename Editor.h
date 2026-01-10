#ifndef TINYTEXTEDITOR_EDITOR_H
#define TINYTEXTEDITOR_EDITOR_H
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
     cursorY_(0)
     {}

   bool CursorAtEndOfLine();
   bool DataExistsAtY(int row);
   std::string_view GetVisibleCharactersAtRow(int i);
   void Draw(std::string_view text);
   void DrawRows();
   void RefreshScreen();
   int ReadKey();
   void MoveCursor(int c);
   void EditorScroll();
   void ProcessKeypress(int c);
   SpecialKey ConvertEscapeKey();

   void ClearScreen();
   void JumpToFirstPixel();
   void UpdateCursorPosition(int x, int y);
   void DrawSplashScreen();
   // bool CheckCharacter(char c);
   void HideCursor();
   void ShowCursor();
   void ClearLineRightOfCursor();

   [[nodiscard]]
   bool ShouldContinue() const { return shouldContinue_; }


private:
   bool         shouldContinue_;
   bool         cursorHidden_;
   EditorState& editorState_;
   AppendBuffer appendBuffer_;
   int          cursorX_;
   int          cursorY_;
   std::string  editorRow_;
   int          numEditorRows_;
};


#endif //TINYTEXTEDITOR_EDITOR_H