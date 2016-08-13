// ============================================================================
// This is a Servlet sample for the G-WAN Web Server (http://www.trustleap.com)
// ----------------------------------------------------------------------------
// noheaders.c: build a complete HTTP reply -without any response header
//              (this useful to send a raw JSON reply for example)
//
//              Returning an INVALID HTTP status code in the 1-99 range 
//              (inclusive) will prevent G-WAN from injecting the missing
//              response HTTP Headers.
// Notes:
//
// This reply is supposed to be fetched by Javascript code running in the 
// browser, see the comet.c example for an illustration.
//
// When the "Content-Length" HTTP header is present, browsers don't have to 
// wait more than the time needed to load the specified amount of data.
//
// The lack of a "Content-Length" HTTP header is forcing Internet Browsers 
// to guess when to stop waiting for more data (they wait for a while, 
// without displaying anything, and then, after a timeout, they display 
// everything they have received).
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN exported functions

// ----------------------------------------------------------------------------
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//    xbuf_cat(): formatted strcat() (a la printf) in a given dynamic buffer 
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // -------------------------------------------------------------------------
   // append your data to the reply buffer
   // -------------------------------------------------------------------------
   char json[] = "{ \"user\" : \"Pierre\" }\n";
   xbuf_ncat(get_reply(argv), json, sizeof(json) - 1);

   // -------------------------------------------------------------------------
   // return an *INVALID* HTTP code (1:RC_NOHEADERS)
   // -------------------------------------------------------------------------
   return RC_NOHEADERS;
}
// ============================================================================
// End of Source Code
// ============================================================================
