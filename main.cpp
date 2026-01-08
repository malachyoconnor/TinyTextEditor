#include "Editor.h"
#include "EditorState.h"

int main() {
   EditorState editorState = EditorState{};

   auto editor = Editor(editorState);

   while (editor.ShouldContinue()) {
      char c = editor.ReadKey();

      editor.RefreshScreen();
      editor.ProcessKeypress(c);
   }

}