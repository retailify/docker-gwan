// ============================================================================
// This is a Servlet sample for the G-WAN Web Server (http://www.trustleap.com)
// ----------------------------------------------------------------------------
// setheaders.c: build a complete HTTP reply -including custom response headers
//               (this may be mandatory for some applications)
//
//               You can insert new response headers to match your needs.
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN exported functions
// ----------------------------------------------------------------------------
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//   xbuf_init(): called after xbuf_t has been declared, to initialize struct
//   xbuf_xcat(): formatted strcat() (a la printf) in a given dynamic buffer 
//     get_env(): get connection's 'environment' variables from the server:
// ----------------------------------------------------------------------------
// a stripped-down* version of itoa() - [*]: no checks, no ending-null
// ----------------------------------------------------------------------------
static inline void u32toa(char *p, u32 v)
{
   do *p-- = '0' + (v % 10), v /= 10; while(v);
}
// ----------------------------------------------------------------------------
// Title of our HTML page
static char title[] = "Setting response headers";

// Top of our HTML page
static char top[] =
     "<!DOCTYPE HTML>"
     "<html lang=\"en\"><head><title>%s</title><meta http-equiv"
     "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
     "<link href=\"imgs/style.css\" rel=\"stylesheet\" type=\"text/css\">"
     "</head><body style=\"margin:16px;\">"
     "<h1>%s</h1>";
 
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // get the current HTTP date (like "Sun, 06 Nov 1994 08:49:37 GMT")
   // -------------------------------------------------------------------------
   char *p = (char*)get_env(argv, SERVER_DATE);

   // -------------------------------------------------------------------------
   // create your custom response headers and append our HTML
   // -------------------------------------------------------------------------
   int position = 0;
   xbuf_xcat(reply,
             "HTTP/1.1 200 OK\r\n"
             "Date: %s\r\n"
             "Last-Modified: %s\r\n"
             
             // << insert your custom headers here
             "X-Powered-by: G-WAN\r\n"
             
             "Content-type: text/html\r\n"
             "Content-Length:      %n\r\n" // make room for the HTML length
             "Connection: close\r\n\r\n",
             p, p,
             &position);  // "Sun, 06 Nov 2009 08:49:37 GMT"
   int headers_size = reply->len;

   // -------------------------------------------------------------------------
   // append your HTML page to the reply buffer
   // -------------------------------------------------------------------------
   xbuf_xcat(reply, top, title, title);
   char szbody[] = "<br>This reply was made with custom HTTP headers,"
                   " look at the servlet source code.<br></body></html>";
   xbuf_ncat(reply, szbody, sizeof(szbody) - 1);

   // -------------------------------------------------------------------------
   // store the HTML size in the empty space of the 'Content-Length' header
   // -------------------------------------------------------------------------
   u32toa(reply->ptr + position, reply->len - headers_size);

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
