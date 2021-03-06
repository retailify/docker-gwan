// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// redirect.c: redirect a client to another URL
//
// Note: if you want to save a new request, you can use set_reply() with a 
//       cached entry (see the /handlers/main_404_redirect.c__ example)
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//   xbuf_xcat(): like sprintf(), but it works in the specified dynamic buffer 
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN exported functions

int main(int argc, char *argv[])
{
   char redir[] = "Location: 100.html\r\n\r\n";
   http_header(HEAD_ADD, redir, sizeof(redir) - 1, argv);

   return HTTP_301_MOVED_PERMANENTLY;
}
// ============================================================================
// End of Source Code
// ============================================================================

