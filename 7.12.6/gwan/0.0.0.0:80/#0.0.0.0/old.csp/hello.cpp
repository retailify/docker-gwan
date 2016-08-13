// ============================================================================
// C++ servlet example for the G-WAN Web Application Server   (http://gwan.ch/)
// ----------------------------------------------------------------------------
// hello.cpp: hello world C++ example
//
// This code was submitted and written by Thomas Meitz (gwan at jproxx dot com)
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//   xbuf_cat(): like strcat(), but it works in the specified dynamic buffer
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN definitions
#include <string>

using namespace std;

class Hello 
{
public: 
   void whatsup(xbuf_t* reply, string& str)
   {
      xbuf_cat(reply, (char*)str.c_str());
   }
};
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   string h("Hello World (C++)");
   Hello hello;
   hello.whatsup(get_reply(argv), h);
   
   return 200; // return an HTTP code (200:'OK')
}
// ============================================================================
// End of Source Code
// ============================================================================

