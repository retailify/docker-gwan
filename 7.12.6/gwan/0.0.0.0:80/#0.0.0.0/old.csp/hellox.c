// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// hellox.c: how to get URL parameters and use them in a reply
//
// used with Lighty's Weighttp to benchmark a minimalist servlet:
// weighttp -c 500 -n 10000 -t 4 -k http://10.10.2.2:8080/cps?hellox&name=Eva
// ============================================================================
// imported functions:
//     get_arg(): get the specified form field (URI parameter) value
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//   xbuf_xcat(): like sprintf(), but it works in the specified dynamic buffer 
// escape_html(): translate HTML tags into harmless escaped sequences
// ----------------------------------------------------------------------------
#include "gwan.h"    // G-WAN exported functions

int main(int argc, char *argv[])
{
   xbuf_t *reply = get_reply(argv);

   char *name = 0;
   get_arg("name=", &name, argc, argv);

   xbuf_xcat(reply, "Hello %H", (name && *name) ? name : "you");
                
   return HTTP_200_OK;
}
// ============================================================================
// End of Source Code
// ============================================================================
