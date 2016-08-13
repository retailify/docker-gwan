// ============================================================================
// Handler C script for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// main.c: filter IP addresses, rewrite URLs, log custom messages, etc.
// ----------------------------------------------------------------------------
// The so_xxxxxx() BSD socket calls, like xbuf_frurl(), are provided by G-WAN 
// for convenience and are using asynchronous client calls, just like the 
// connnect() / send() / recv() system calls that you can use directly: G-WAN
// transparently transforms blocking system calls into asynchronous calls, for
// C scripts (servlets, handlers) as well as for (shared or static) libraries 
// linked with C scripts by "#pragma link".
// ============================================================================
//#pragma debug     // uncomment to get symbols/line numbers in crash reports

#include "gwan.h"   // G-WAN exported functions

#include <stdio.h>  // printf()
#include <string.h> // strcmp()

// ----------------------------------------------------------------------------
// structure holding pointers for persistence
// ----------------------------------------------------------------------------
typedef struct 
{ 
   kv_t *kv;   // a Key-Value store
   char *blah; // a string
   int   val;  // a counter
} data_t;

// ----------------------------------------------------------------------------
// init() will initialize your data structures, load your files, etc.
// ----------------------------------------------------------------------------
// init() should return -1 if failure (to allocate memory for example)
int init(int argc, char *argv[])
{
   // get the Handler persistent pointer to attach anything you need
 //data_t **data = (data_t**)get_env(argv, US_HANDLER_DATA);
 //data_t **data = (data_t**)get_env(argv, US_VHOST_DATA);
   data_t **data = (data_t**)get_env(argv, US_SERVER_DATA);
   
   // initialize the persistent pointer by allocating our structure
   // attach structures, lists, sockets with a back-end/database server, 
   // file descriptiors for custom log files, etc.
   *data = (data_t*)calloc(1, sizeof(data_t));
   if(!*data)
      return -1; // out of memory

   // define which handler states we want to be notified in main():
   // enum HANDLER_ACT { 
   //  HDL_INIT = 0, 
   //  HDL_AFTER_ACCEPT, // just after accept (only client IP address setup)
   //  HDL_AFTER_READ,   // each time a read was done until HTTP request OK
   //  HDL_BEFORE_PARSE, // HTTP verb/URI validated but HTTP headers are not 
   //  HDL_AFTER_PARSE,  // HTTP headers validated, ready to build reply
   //  HDL_BEFORE_WRITE, // after a reply was built, but before it is sent
   //  HDL_HTTP_ERRORS,  // when G-WAN is going to reply with an HTTP error
   //  HDL_CLEANUP };
   u32 *states = get_env(argv, US_HANDLER_STATES);
   *states = (1L << HDL_AFTER_ACCEPT) 
           | (1L << HDL_AFTER_READ)
           | (1L << HDL_BEFORE_WRITE);
   
   return 0;
}
// ----------------------------------------------------------------------------
// clean() will free any allocated memory and possibly log summarized stats
// ----------------------------------------------------------------------------
void clean(int argc, char *argv[])
{
   // free any data attached to your persistence pointer
   data_t *data = (data_t*)get_env(argv, US_SERVER_DATA);

   // we could close a data->log custom file 
   // if the structure had a FILE *log; field
   // fclose(data->log);

   if(data)
      free(*data);
}
// ----------------------------------------------------------------------------
// main() does the job for all the connection states below:
// (see 'HTTP_Env' in gwan.h for all the values you can fetch with get_env())
// ----------------------------------------------------------------------------

// those defines summarise the return codes for each state
#define RET_CLOSE_CONN   0 // all states
#define RET_BUILD_REPLY  1 // HDL_AFTER_ACCEPT only
#define RET_READ_MORE    1 // HDL_AFTER_READ only
#define RET_SEND_REPLY   2 // all states
#define RET_CONTINUE   255 // all states

int main(int argc, char *argv[])
{
   // get the Handler persistent pointer if we attached anything to it
   data_t *data = (data_t*)get_env(argv, US_SERVER_DATA);

   // just helping you to know where you are:
   static char *states[] =
   {
      [HDL_INIT]         = "0:init()",      // not seen here: init()
      [HDL_AFTER_ACCEPT] = "1:AfterAccept",
      [HDL_AFTER_READ]   = "2:AfterRead",
      [HDL_BEFORE_PARSE] = "3:BeforeParse",
      [HDL_AFTER_PARSE]  = "4:AfterParse",
      [HDL_BEFORE_WRITE] = "5:BeforeWrite",
      [HDL_AFTER_WRITE]  = "6:AfterWrite",
      [HDL_HTTP_ERRORS]  = "7:HTTP_Errors",
      [HDL_CLEANUP]      = "8:clean()",   // not seen here: clean()
      ""
   };
   long state = (long)argv[0];
   printf("Handler state:%ld:%s\n", state, states[state]);
   
   switch(state)
   {
      // ----------------------------------------------------------------------
      // AFTER_ACCEPT return values:
      //   0: Close the client connection
      //   1: Build a reply based on a custom request buffer/HTTP status code
      //   2: Send a server reply based on a reply buffer/HTTP status code
      // 255: Continue (wait for read() to fetch data sent by client)
      case HDL_AFTER_ACCEPT:
      {
         char *szIP = (char*)get_env(argv, REMOTE_ADDR);
         
         // we don't want this user to touch our server
         if(!szIP || !strcmp(szIP, "1.2.3.4"))
            return 0; // 0: Close the client connection

         // we want this other user to be redirected to another server
         if(!strcmp(szIP, "5.6.7.8"))
         {
            char szURI[] = "http://another-place.org";
            xbuf_t *reply = get_reply(argv);
            xbuf_xcat(reply,
                      "<html><head><title>Redirect</title></head>"
                      "<body>Click <a href=\"%s\">here</a>.</body></html>",
     					    szURI);

            // set the HTTP reply code accordingly
            int *pHTTP_status = (int*)get_env(argv, HTTP_CODE);
            if(pHTTP_status)
               *pHTTP_status = 301; // 301:'moved permanently'
               
            // 2: Send a server reply based on a reply buffer/HTTP status code
            return 2;
         }
      }
      break;
      // ----------------------------------------------------------------------
      // AFTER_READ return values:
      //   0: Close the client connection
      //   1: Read more data from client
      //   2: Send a server reply based on a reply buffer/HTTP status code
      // 255: Continue (read more if request not complete or build reply based
      //                on the client request -or your altered version)
      case HDL_AFTER_READ:
      {
         xbuf_t *read_buffer = (void*)get_env(argv, READ_XBUF);
         // here we could bypass HTTP and do something else
         // (note that the request might not be completely loaded)
      }
      break;
      // ----------------------------------------------------------------------
      // BEFORE_PARSE return values:
      //   0: Close the client connection
      //   2: Send a server reply based on a reply buffer/HTTP status code
      // 255: Continue (parse the request and its HTTP Headers)
      case HDL_BEFORE_PARSE:
      {
         xbuf_t *read_buffer = (void*)get_env(argv, READ_XBUF);
         // do something with the request (re-write URL? do it in-place)
         // note that if the request is very long (like a POST entity)
         // then the whole request might be completely loaded only at the
         // HDL_AFTER_PARSE state
      }
      break;
      // ----------------------------------------------------------------------
      // AFTER_PARSE return values:
      //   0: Close the client connection
      //   2: Send a server reply based on a reply buffer/HTTP status code
      // 255: Continue (build a reply)
      case HDL_AFTER_PARSE:
      {
         char  *szRequest = (void*)get_env(argv, REQUEST);
         http_t *headers =  (void*)get_env(argv, HTTP_HEADERS);
         // here we can take action based on HTTP Headers, to alter the 
         // request or bypass G-WAN's default behavior
      }
      break;
      // ----------------------------------------------------------------------
      // HTTP_ERRORS return values:
      //   0: Close the client connection
      //   2: Send a server reply based on a custom reply buffer
      // 255: Continue (send a reply based on the request HTTP code)
      case HDL_HTTP_ERRORS:
      {
         // here we could use our data->log file to log custom events
         // char string[256];
         // s_snprintf(string, sizeof(string)-1, "whatever", x,y,z);
         // fputs(string, data->log);
      }
      break;
      // ----------------------------------------------------------------------
      // BEFORE_WRITE return values:
      //   0: Close the client connection
      // 255: Continue (send a server reply based on a reply buffer/HTTP code)
      case HDL_BEFORE_WRITE:
      {
         // here we could use our data->log file to log custom events
         // char string[256];
         // s_snprintf(string, sizeof(string)-1, "whatever", x,y,z);
         // fputs(string, data->log);
      }
      break;
      // ----------------------------------------------------------------------
      // AFTER_WRITE return values:
      //   0: Close the client connection
      // 255: Continue (close or keep-alive or continue sending a long reply)
      case HDL_AFTER_WRITE:
      {
         // added per the request of someone
      }
      break;
   }
   return(255); // continue G-WAN's default execution path
}
// ============================================================================
// End of Source Code
// ============================================================================
