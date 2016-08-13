// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// cache?.c: demonstrates how to add a servlet output into G-WAN's cache,
//           using an user-defined URI (not "/?cache")
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//    xbuf_cat(): like strcat(), but it works in the specified dynamic buffer 
//    cacheadd(): puts the specified contents into G-WAN's cache
// ----------------------------------------------------------------------------
#include "gwan.h"    // G-WAN exported functions

#include <stdio.h>

int main(int argc, char *argv[])
{
   xbuf_t *reply = get_reply(argv);

   xbuf_cat(reply, "<h1>Cache entry 1</h1>");

   static char szpath[] = "cache1.html";
   int expire = 15;
	if(!cacheadd(argv, szpath, reply->ptr, reply->len, ".html", 200, expire))
   {
      printf("*** error: could not add cache entry #1\n");
      return HTTP_503_SERVICE_UNAVAILABLE;
   }   

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
