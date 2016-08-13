// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// stream1.c: how to send *partial* replies to a client, as fast as possible
//            (the servlet is called repeatedly as soon as the previous reply
//             has been sent to the client). Output:
//
//            partial reply 1
//            partial reply 2
//            partial reply 3
//
// To send contents of variable length (that you can't define in HTTP headers
// as you don't know it in advance) use chunked encoding (see comet.c).
//
// See stream2.c and stream3.c for more sophisticated examples involving delays
// and external sources (database, system command, etc.).
// ============================================================================
#include "gwan.h"

#define NBR_CHUNKS 10

int main(int argc, char *argv[])
{
   // get the server 'reply' buffer where to write our answer to the client
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // step 1: setup a per-request context
   // -------------------------------------------------------------------------
   void **data = (void**)get_env(argv, US_REQUEST_DATA);
   if(!data[0]) // we did not setup our per-request counter yet
   {
      data[0] = (void*)1; // use the persistent pointer as a simple counter
      
      char head[] = "HTTP/1.1 200 OK\r\n"
                    "Connection: close\r\n"
                    "Content-length: 60\r\n" // 3 * 20 characters
                    "Content-type: text/plain; charset=utf-8\r\n"
                    "\r\n\r\n";
      xbuf_ncat(reply, head, sizeof(head) - 1);
   }

   // -------------------------------------------------------------------------
   // step 2: repeatedly send to client an incremental reply
   // -------------------------------------------------------------------------
   xbuf_xcat(reply, "partial reply %llu<br>\n", data[0]++);

   // -------------------------------------------------------------------------
   // 3: decide when to stop streaming
   // -------------------------------------------------------------------------
   if(data[0] > (void*)NBR_CHUNKS)
   {
      data[0] = 0;
      return RC_NOHEADERS; // do not generate HTTP headers
   }

   // RC_NOHEADERS: do not generate HTTP headers
   // RC_STREAMING: call me again after send() is done
   return RC_NOHEADERS + RC_STREAMING; 
}
// ============================================================================
// End of Source Code
// ============================================================================

