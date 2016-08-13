// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// extern.c: how to invoke an external program from a G-WAN C servlet
//
// This is the easy way to call Linux tools like ping, traceroute, whois, etc.
// but it blocks the worker thread for the duration of the called process.
//
// For the proper (non-blocking) way of doing this (using incremental updates)
// see the stream3.c example.
// ============================================================================
#include "gwan.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
   char readbuf[1024];

   // -------------------------------------------------------------------------
   // create an output filename
   // -------------------------------------------------------------------------
   char *www = (char*)get_env(argv, WWW_ROOT);
   char filename[512];
   s_snprintf(filename, sizeof(filename) - 1, "%s/result.txt", www);

   // -------------------------------------------------------------------------
   // get the server 'reply' buffer where to write our answer to the client
   // -------------------------------------------------------------------------
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // run the "ping -c 1 127.0.0.1" command
   // -------------------------------------------------------------------------
   FILE *fpipe = popen("ping -c 1 127.0.0.1 2>&1", "r");
   if(!fpipe) 
   {
      int ret = strerror_r(errno, readbuf, sizeof(readbuf));
      xbuf_cat(reply, ret ? "unknown popen() error" : readbuf);
      return HTTP_200_OK;
   }

   // -------------------------------------------------------------------------
   // fetch the command output and store it in the 'reply' buffer
   // -------------------------------------------------------------------------
   xbuf_ncat(reply, "<pre>", sizeof("<pre>") - 1);
   for(;;)
   {
       int len = fread(readbuf, 1, sizeof(readbuf), fpipe);
       if(len <= 0) break;
       xbuf_ncat(reply, readbuf, len);
   }
   pclose(fpipe);
   xbuf_ncat(reply, "</pre>\n\n", sizeof("</pre>\n\n") - 1);

   // -------------------------------------------------------------------------
   // do something with the output
   // -------------------------------------------------------------------------
   puts(reply->ptr);             // display in terminal
   xbuf_tofile(reply, filename); // save as a file

   // -------------------------------------------------------------------------
   // the server 'reply' will also show the command output
   // -------------------------------------------------------------------------
   return HTTP_200_OK;
}
// ============================================================================
// End of Source Code
// ============================================================================

