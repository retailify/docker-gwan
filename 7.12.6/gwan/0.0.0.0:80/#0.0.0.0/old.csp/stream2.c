// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// stream2.c: how to send *partial* replies to a client, each after a second
//            (the servlet is called repeatedly as soon as the specified delay 
//             has elapsed). Output:
//
//            partial reply 1
//            partial reply 2
//            partial reply 3
//
// To send contents of variable length (that you can't define in HTTP headers
// as you don't know it in advance) use chunked encoding (see comet.c).
//
// See stream3.c to use a child command started through a pipe and a request
// context to maintain the state of the on-going (non-blocking) streaming.
// ============================================================================
#include "gwan.h"

#define NBR_CHUNKS 10

int main(int argc, char *argv[])
{
   // get the server 'reply' buffer where to write our answer to the client
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // step 1: setup a per-request context, and a call periodicity
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
      
      // ----------------------------------------------------------------------
      // tell G-WAN when to run this script again (for the same request)
      // ----------------------------------------------------------------------
      wake_up(argv, 1000, WK_MS, 0); // WK_MS:milliseconds, WK_FD:file_descriptor
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
      wake_up(argv, 0, WK_MS, 0); // first 0:no more wake-up
      return RC_NOHEADERS; // do not generate HTTP headers
   }

   // RC_NOHEADERS: do not generate HTTP headers
   // RC_STREAMING: call me again after send() is done
   return RC_NOHEADERS + RC_STREAMING; 
}
// ============================================================================
// End of Source Code
// ============================================================================

