#ifndef TINYTEXTEDITOR_APPENDBUFFER_H
#define TINYTEXTEDITOR_APPENDBUFFER_H
#include <iostream>
#include <string>
#include <unistd.h>

class AppendBuffer {
public:
   AppendBuffer() = default;

   void Append(const std::string_view str) {
      buffer_ += str;
   }

   void Append(const char str) {
      buffer_ += str;
   }

   void WriteBufferToSTDOUT() {
      if (buffer_.empty()) return;

      write(STDOUT_FILENO, buffer_.c_str(), buffer_.size());
   }

   void ClearBuffer() {
      buffer_ = "";
   }

private:
   std::string buffer_;
};


#endif //TINYTEXTEDITOR_APPENDBUFFER_H