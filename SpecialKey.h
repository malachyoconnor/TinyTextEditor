#ifndef TINYTEXTEDITOR_KEYPRESS_H
#define TINYTEXTEDITOR_KEYPRESS_H

enum SpecialKey {
   ESCAPE_CHAR = '\x1b',
   BACKSPACE = 127,


   // Outside the range of a char
   ARROW_LEFT = 1000,
   ARROW_DOWN,
   ARROW_RIGHT,
   ARROW_UP,
   DELETE,
   END,
   HOME,
   PAGE_DOWN,
   PAGE_UP,
 };

#endif //TINYTEXTEDITOR_KEYPRESS_H