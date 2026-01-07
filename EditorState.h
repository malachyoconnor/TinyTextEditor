#ifndef TINYTEXTEDITOR_EDITORSTATE_H
#define TINYTEXTEDITOR_EDITORSTATE_H
#include <optional>
#include <termios.h>
#include "AppendBuffer.h"


class EditorState {
public:
   EditorState();

private:
   int screenRows = -1;
   int screenCols = -1;
   std::optional<termios> initialTerminalAttributes = std::nullopt;

   AppendBuffer buf;
};

#endif //TINYTEXTEDITOR_EDITORSTATE_H