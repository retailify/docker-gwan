// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// comet.c: AJAX 'Comet' streaming of stock values (publisher/subscriber)
//
//      The 'livestock' form sends an initial GET/POST query to G-WAN, which
//      then sends replies at the requested interval to the client (until the
//      connection is closed - either by the client or the server).
//
//      Each client can request a different push frequency. The lowest delay 
//      is used by G-WAN to update this data with the make_fn() callback.
//
//      Only the Stock that has changed on the server side is transmitted. All 
//      is event-based, so G-WAN worker threads never block (making it possible
//      to feed many of such clients per second while other types of requests 
//      are satisfied by the server).
// ============================================================================
#include "gwan.h"  // G-WAN exported functions

#include <stdio.h> // puts()
#include <stdlib.h>
#include <string.h>

#define FEED_NAME "livestock" // the feed served by this servlet

// ----------------------------------------------------------------------------
// the data we make and push
// ----------------------------------------------------------------------------
#define ROWS  4 
#define COLS  4
#define CELLS (ROWS * COLS)
static float *s_data = 0; // the most efficient way to store a 2D array is 1D
static prnd_t rnd;

// ----------------------------------------------------------------------------
// the callback used by G-WAN to generate the data pushed to clients
// ----------------------------------------------------------------------------
static int make_fn(char *argv[])
{
   // of course, instead of using pseudo-random data, one could agregate data
   // from a database back-end server, or from another application server, or 
   // use other client requests that bring new data (publishers) but we could
   // also calculate complex things here (scoring, simulations, etc.)
   //
   // the point is to do things once (here) for all the clients we feed 
   // instead of doing it repeatedly each time a client gets data
   //
   if(!s_data)
   {
      s_data = (float*)calloc(1, sizeof(float) * CELLS);
      if(!s_data)
         return 0;
   }

   static int once = 0;
   if(!once)
   {
      once = 1;
      sw_init(&rnd, (u32)getns()); // (period: 1 << 158)
   }
   
   int i = 0;
   while(i < CELLS)
      s_data[i++] = (sw_rand(&rnd) % 100000) / 100.;

   return 1;
}
// ----------------------------------------------------------------------------
// callback used to free data and to delete the feed
// (this is hapening if make_fn() or push_fn() return 0)
// ----------------------------------------------------------------------------
static void free_fn(char *argv[])
{
   if(s_data)
   {
      free(s_data);
      s_data = 0;
   }
}
// ----------------------------------------------------------------------------
// the callback used to format data (into text, json, html, etc.) for clients
// ----------------------------------------------------------------------------
// HTTP/1.1 200 OK
// Content-type:text/plain
// Transfer-Encoding: chunked
// 
// 3c;     
// KEY:time VAL:Mon, 12 Mar 2012 13:11:49 GMT|KEY:00 VAL:14.03
// 3c;     
// KEY:time VAL:Mon, 12 Mar 2012 13:11:49 GMT|KEY:00 VAL:40.75
// 3c;     
// KEY:time VAL:Mon, 12 Mar 2012 13:11:49 GMT|KEY:00 VAL:45.02
// 0   
//
static int push_fn(char *argv[], xbuf_t *reply)
{
   reply->len = 0; // empty buffer
   xbuf_xcat(reply, "        \r\n" // room for chunck size: "1a;     "
             "KEY:time VAL:%s|", (char*)get_env(argv, SERVER_DATE));
             
   // fill the 2D HTML <table> cells with our 1D array
   // (we randomly skip some cells to avoid updating all of them)
   {
      int i = 0, r = 0, c = 0;
      while(i < CELLS)
      {
          if(!(sw_rand(&rnd) & 3)) // skip some cells
             xbuf_xcat(reply, "KEY:%c%c VAL:%.02f|",
                              '0' + r, '0' + c, s_data[i++]);

          if(c + 1 < COLS) c++; 
          else
          {
             c = 0;
             if(r + 1 < ROWS) r++; else r = 0;
          }
      }
   }   
   reply->ptr[--reply->len] = 0; // remove the ending '|'
   
   // now we know it, setup the chunk size
   char *p = reply->ptr;
   int i = s_snprintf(p, 8, "%x;", reply->len - (sizeof("        \r\n") - 2));
   p[i] = ' '; // useful for tracing, when we puts(reply->ptr); later
   
   xbuf_ncat(reply, "\r\n", sizeof("\r\n") - 1); // close this chunk
   return 1;   
}
// ============================================================================
// main() is receiving the query parameters ("csp?arg1&arg2&arg3...") in argv[]
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // create a dynamic buffer and get a pointer on the server response buffer
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // filter input data to avoid all the useless/nasty cases
   // -------------------------------------------------------------------------
   char *Feed  = ""; get_arg("feed=",  &Feed, argc, argv);
   char *Delay = ""; get_arg("delay=", &Delay, argc, argv);

   // -------------------------------------------------------------------------
   // no valid parameters provided, redirect client to "csp_comet.html"
   // -------------------------------------------------------------------------
   if(argc < 1 || !Delay[0] 
   || !Feed[0] || memcmp(Feed, FEED_NAME, sizeof(FEED_NAME)))
   {
      static char redir[] = "HTTP/1.1 302 Found\r\n"
         "Content-type:text/plain\r\n"
         "Location: /csp_comet.html\r\n\r\n"
         "<html><head><title>Redirect</title></head><body>"
         "Click <a href=\"/csp_comet.html\">here</a>.</body></html>";

      xbuf_ncat(reply, redir, sizeof(redir) - 1);
      return HTTP_302_MOVED_TEMPORARILY; // return an HTTP code
   }
   
   int delay = atoi(Delay);
   if(delay <  1) delay = 1; else
   if(delay > 10) delay = 10; // arbitrary choice for this demo

   // -------------------------------------------------------------------------
   // make this client subscribe to the requested feed
   // -------------------------------------------------------------------------
   // this also creates the feed if it does not exist already
   {
      // internally, make_freq is never > push_freq; here we just define a
      // by-default value aimed at not loading the server pointlessly
      u32 make_freq = 1, push_freq = delay;
      if(!push_list_add(argv, 
                        FEED_NAME, 
                        make_fn, make_freq, // 1 second here (easy to see)
                        push_fn, push_freq, // user-defined
                        free_fn))
         return HTTP_503_SERVICE_UNAVAILABLE;
   }
   
   // -------------------------------------------------------------------------
   // send the "Transfer-Encoding: chunked" HTTP header
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
   {
      char head[] = "HTTP/1.1 200 OK\r\n"
                  //"Connection: close\r\n"
                  //"Content-type: application/json; charset=utf-8\r\n"
                    "Content-type: text/plain; charset=utf-8\r\n"
                    "Transfer-Encoding: chunked\r\n\r\n";
      xbuf_ncat(reply, head, sizeof(head) - 1);
   }
   
   // -------------------------------------------------------------------------
   /* send first frame
   // -------------------------------------------------------------------------
   xbuf_t buf;
   xbuf_init(&buf);
   make_fn(argv);   
   push_fn(argv, &buf);
   xbuf_ncat(reply, buf.ptr, buf.len);
   xbuf_free(&buf); */
   
   //puts(reply->ptr);
   return HTTP_200_OK; 
}
// ============================================================================
// End of Source Code
// ============================================================================
/*
// count rows in a table
var oRows = document.getElementById('MyTable').getElementsByTagName('tr');
var iRowCount = oRows.length;
alert('Your table has ' + iRowCount + ' rows.');

// count columns in a table
cols = document.getElementById('tablename').rows[0].cells.length;

// add cell in a row
var x=document.getElementById('tr1').insertCell(0);
x.innerHTML="The famous";

// insert ROWS to a table:
var table=document.getElementById("myTable");
var row=table.insertRow(0);
var cell1=row.insertCell(0);
var cell2=row.insertCell(1);
cell1.innerHTML="New";
cell2.innerHTML="New";

// delete ROW
document.getElementById("myTable").deleteRow(0);

// add caption
var x=document.getElementById("myTable").createCaption();
x.innerHTML="<b>My table caption</b>";

// delete caption
document.getElementById("myTable").deleteCaption();

// borders
document.getElementById('myTable').frame="above";
document.getElementById('myTable').frame="below";
document.getElementById('myTable').rules="cols";
document.getElementById('myTable').rules="rows";
document.getElementById('myTable').border="10";
*/        


/*

<div id="example"></div>

<script type="text/javascript">

txt = "<p>Browser CodeName: " + navigator.appCodeName + "</p>";
txt+= "<p>Browser Name: " + navigator.appName + "</p>";
txt+= "<p>Browser Version: " + navigator.appVersion + "</p>";
txt+= "<p>Cookies Enabled: " + navigator.cookieEnabled + "</p>";
txt+= "<p>Platform: " + navigator.platform + "</p>";
txt+= "<p>User-agent header: " + navigator.userAgent + "</p>";

document.getElementById("example").innerHTML=txt;

</script>


Browser CodeName: Mozilla
Browser Name: Netscape
Browser Version: 5.0 (X11)
Cookies Enabled: true
Platform: Linux i686 on x86_64
User-agent header: Mozilla/5.0 (X11; Linux i686 on x86_64; rv:10.0) Gecko/20100101 Firefox/10.0
*/
