#include "Editor.h"
#include "EditorState.h"

int main(int argc, char** argv) {
   EditorState editorState = EditorState{};

   auto editor = Editor(editorState);
   editor.SetHelpMessage("Press Ctrl + Q to exit");

   if (argc > 1) {
      editorState.OpenFile(argv[1]);
   }

   while (editor.ShouldContinue()) {
      char c = editor.ReadKey();

      editor.RefreshScreen();
      editor.ProcessKeypress(c);
   }

}