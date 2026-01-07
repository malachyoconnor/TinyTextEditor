#ifndef TINYTEXTEDITOR_EDITOR_H
#define TINYTEXTEDITOR_EDITOR_H
#include "EditorState.h"


class Editor {
public:
   Editor(EditorState& editorState) : shouldContinue_(true), editorState_(editorState) {
   }

   void DrawRows();
   void RefreshScreen();
   char ReadKey();
   void ProcessKeypress(char c);

   [[nodiscard]]
   bool ShouldContinue() const { return shouldContinue_; }


private:
   bool shouldContinue_;
   EditorState& editorState_;
};


#endif //TINYTEXTEDITOR_EDITOR_H