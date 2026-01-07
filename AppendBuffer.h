#ifndef TINYTEXTEDITOR_APPENDBUFFER_H
#define TINYTEXTEDITOR_APPENDBUFFER_H
#include <cstring>
#include <string>


class AppendBuffer {
public:

   AppendBuffer() {
      len = 0;
   }

   void Append(const std::string& str) {
      char* newBuf = static_cast<char*>(realloc(b, len + str.size()));

      if (newBuf == nullptr) return;
      memcpy(&newBuf[len], str.c_str(), str.size());
      b = newBuf;
      len += str.size();
   }

   ~AppendBuffer() {
      free(b);
   }

private:
   char* b;
   int len;
};


#endif //TINYTEXTEDITOR_APPENDBUFFER_H