#ifndef TINYTEXTEDITOR_APPENDBUFFER_H
#define TINYTEXTEDITOR_APPENDBUFFER_H
#include <cstring>
#include <string>

class AppendBuffer {
public:
   AppendBuffer() : buffer_(nullptr), len_(0) {
   }

   void Append(const std::string& str) {
      char* newBuf = static_cast<char*>(realloc(buffer_, len_ + str.size()));

      if (newBuf == nullptr) return;
      memcpy(&newBuf[len_], str.c_str(), str.size());
      buffer_ = newBuf;
      len_ += str.size();
   }

   ~AppendBuffer() {
      if (buffer_ != nullptr) free(buffer_);
   }

private:
   char* buffer_;
   int   len_;
};


#endif //TINYTEXTEDITOR_APPENDBUFFER_H