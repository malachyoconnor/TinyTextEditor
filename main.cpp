#include <iostream>
#include <unistd.h>
#include <termios.h>

#include "AppendBuffer.h"
#include "Editor.h"
#include "EditorState.h"
#include "utils.h"


int main() {
   EditorState editorState = EditorState{};
   editorState.EnableRawMode();

   auto editor = Editor(editorState);

   while (editor.ShouldContinue()) {
      char c = editor.ReadKey();

      editor.RefreshScreen();
      editor.ProcessKeypress(c);
   }

}