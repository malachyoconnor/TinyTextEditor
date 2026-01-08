#ifndef TINYTEXTEDITOR_KEYPRESS_H
#define TINYTEXTEDITOR_KEYPRESS_H

enum SpecialKey {
   ESCAPE_CHAR = '\x1b',

   // Outside the range of a char
   ARROW_LEFT = 1000,
   ARROW_RIGHT,
   ARROW_UP,
   ARROW_DOWN,
   PAGE_UP,
   PAGE_DOWN,
   HOME,
   END,
 };

#endif //TINYTEXTEDITOR_KEYPRESS_H