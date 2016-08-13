// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// stream3.c: how to pass external program *incremental* replies to a client
//
//           running 'ping -c 3 127.0.0.1' provides the following output:
//
//           PING 127.0.0.1 (127.0.0.1) 56(84) bytes of data.
//           64 bytes from 127.0.0.1: icmp_req=1 ttl=64 time=0.022 ms
//           64 bytes from 127.0.0.1: icmp_req=2 ttl=64 time=0.014 ms
//           64 bytes from 127.0.0.1: icmp_req=3 ttl=64 time=0.016 ms
//
//           Each ping is received at a 1 second interval so the whole test
//           lasts 3 seconds. But it could last 20 seconds, or even 1 hour.
//
//           We don't want our servlet to block during this time, so we are
//           *streaming* each partial reply to the client (instead of waiting
//           for the command to complete and then sending the whole result).
//
//           For Internet Browsers to cooperate, we need to send chunked data
//           because the lack of a "Content-Length" HTTP header will force them
//           to "guess" when to stop waiting for more data (they would wait for
//           a while, without displaying anything, and then, after a timeout,
//           they would display everything they have received).
//
//           To have a context attached to this specific request we are using
//           the gc_init() call to create a memory pool. It is automatically
//           deleted by G-WAN when the request ends (when the reply has been 
//           sent to the client, or if the client closed the connection).
//
//           Because we are using a very simple context, the memory pool is 
//           wasting memory (memory pools can't be smaller than PAGE_SIZE,
//           which is usually 4096 bytes) but that's the occasion to show 
//           how memory pools work.
//
// That's the proper way to call Linux tools like ping, traceroute, whois,
// database queries, back-end queries and other sorts of long-lasting requests.
// ============================================================================
#include "gwan.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NBR_CHUNKS "10" // a 10-second ping

// ----------------------------------------------------------------------------
// our (minimalist) per-request context
// ----------------------------------------------------------------------------
typedef struct { int f; } data_t;

int main(int argc, char *argv[])
{
   char readbuf[1024] = {0};

   // get the server 'reply' buffer where to write our answer to the client
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // step 1: setup a per-request context
   // -------------------------------------------------------------------------
   data_t **data = (void*)get_env(argv, US_REQUEST_DATA);
   if(!*data) // we did not setup our per-request structure yet
   {
      // create a per-request memory pool
      if(!gc_init(argv, 4070)) // we can call gc_alloc() to consume 4070 bytes
         return HTTP_503_SERVICE_UNAVAILABLE; // could not allocate memory!
      
      *data = gc_malloc(argv, sizeof(data_t)); // allocate our context
      if(!*data) return HTTP_503_SERVICE_UNAVAILABLE; // better safe than sorry
      
      // ----------------------------------------------------------------------
      // step 2: run an asynchronous (or incremental) job
      // ----------------------------------------------------------------------
      // run the "ping -c 10 127.0.0.1" command
      // (run_cmd() replaces GLIBC's popen() which uses bufferring)
      char *args[] = { "-c", NBR_CHUNKS, "127.0.0.1" };
      (*data)->f = run_cmd("ping", sizeof(args) / sizeof(args[0]), args);
      if((*data)->f == 0) // error
      {
         int ret = strerror_r(errno, readbuf, sizeof(readbuf));
         xbuf_cat(reply, ret ? "unknown error" : readbuf);
         return HTTP_200_OK;
      }
      
      // ----------------------------------------------------------------------
      // tell G-WAN when to run this script again (for the same request)
      // ----------------------------------------------------------------------
      wake_up(argv, (*data)->f, WK_FD, 0); // when fd buffer has data
      
      // ----------------------------------------------------------------------
      // send chunked encoding HTTP header and HTTP status code
      // ----------------------------------------------------------------------
      char head[] = "HTTP/1.1 200 OK\r\n"
                    "Connection: close\r\n"
                    "Content-type: text/html; charset=utf-8\r\n"
                    "Transfer-Encoding: chunked\r\n\r\n"
                    "5\r\n<pre>\r\n";
      xbuf_ncat(reply, head, sizeof(head) - 1);
   }

   // -------------------------------------------------------------------------
   // step 3: repeatedly read (and send to client) ping's incremental reply
   // -------------------------------------------------------------------------
   // fetch the command output and store it in the 'reply' buffer
   const int len = read((*data)->f, readbuf, sizeof(readbuf));
   if(len <= 0) // done
   {
      close((*data)->f);
      
      // note that malloc() would have to free the 'data' context here
      // (gc_malloc() is automatically freed, along with its memory pool
      //  so there's no need for an explicit free)

      // end the on-going chunked encoding
      char end[] = "6\r\n</pre>\r\n0\r\n\r\n";
      xbuf_ncat(reply, end, sizeof(end) - 1);
      
      wake_up(argv, 0, WK_FD, 0); // 0:no more wake-up please
      
      return RC_NOHEADERS; // RC_NOHEADERS: do not generate HTTP headers
   }

   // -------------------------------------------------------------------------
   // format reply to use chunked encoding
   // -------------------------------------------------------------------------
   // anatomy of a chunked response:
   // 
   //  HTTP/1.1 200 OK [CRLF]                          <-+
   //  Content-Type: text/html [CRLF]                    | HTTP headers
   //  Transfer-Encoding: chunked [CRLF]               <-+
   //  [CRLF]
   //  1a; optional-stuff-here [CRLF]                  // hexadecimal length
   //  abcdefghijklmnopqrstuvwxyz [CRLF]               // data (ASCII/binary)
   //  10 [CRLF]                                       // hexadecimal length
   //  1234567890abcdef [CRLF]                         // data (ASCII/binary)
   //  0 [CRLF]                                        // 0: end of chunks
   //  optional-footer: some-value [CRLF]              // can be HTTP headers
   //  optional-another-footer: another-value [CRLF]   // can be HTTP headers
   //  [CRLF]
   
   xbuf_xcat(reply, "%x\r\n%s\r\n", len, readbuf);

   // -------------------------------------------------------------------------
   // return code
   // -------------------------------------------------------------------------
   // RC_NOHEADERS: do not generate HTTP headers
   // RC_STREAMING: call me again after send() is done
   return RC_NOHEADERS + RC_STREAMING; 
}
// ============================================================================
// End of Source Code
// ============================================================================

