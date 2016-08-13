// ---------------------------------------------------------------------------
// This test is in relation with the "Bigtable" tests done in Haskell: 
//
//    http://jaspervdj.be/blaze/benchmarks.html
//
// Here is a comparison of Haskell a "high-performance implementation of 
// concurrency and parallelism" (according to Wikipedia) to plain old C:
//
//    http://shootout.alioth.debian.org/u32/benchmark.php?test=all&lang=ghc&lang2=gcc 
//
// For the G-WAN + C script test below, I kept the first (redundant) loop 
// since it is so pointless that I assumed that it must be part of the 
// experiment.
//
// I mainly tried to use the G-WAN API in a better way, avoiding data copies
// when possible.
//
// I doubt that my rewrite makes that much difference in a benchmark however:
//
// One single request takes 0.8 milliseconds. Due to multi-threading and CPU
// cache coherence, the concurrency test below is even more impressive, G-WAN
// spits 315,798 requests/s on a 6-Core CPU.
//
// weighttp -n 1000000 -c 100 -t 6 -k -H "Accept-Encoding: gzip" / 
// "http://127.0.0.1:8080/?bigtable.c"
//
// finished in 3 sec, 166 millisec and 580 microsec, 315798 req/s, 243301 kbyte/s
// requests: 1000000 total/started/done/succeeded, 0 failed/errored
// status codes: 1000000 2xx, 0 3xx, 0 4xx, 0 5xx
// traffic: 788922837 bytes total, 301999935 bytes http, 486922902 bytes data
// ---------------------------------------------------------------------------
#include "gwan.h"

#define ROWS 1000
#define COLS   10

int main(int argc, char *argv[])
{
   int i, j;
   int table[ROWS][COLS];
   u8 buffer[8];
   xbuf_t *reply = get_reply(argv);

   u64 start = getus(); // getms() milliseconds, getns() nanoseconds
   for(i = 0; i < ROWS; i++)
   {
      for(j = 0; j < COLS; j++)
         table[i][j] = j + 1;
   }

   xbuf_ncat(reply, "<table>", sizeof("<table>") - 1);
   for(i = 0; i < ROWS; i++)
   {
      xbuf_ncat(reply, "<tr>", sizeof("<tr>") - 1);
      for(j = 0; j < COLS; j++)
      {
         s_snprintf((char*)buffer, sizeof(buffer) - 1, "%d", table[i][j]);
         escape_html(buffer, buffer, sizeof(buffer) - 1);
         xbuf_xcat(reply, "<td>%s</td>", buffer);
      }
      xbuf_ncat(reply, "</tr>", sizeof("</tr>") - 1);
   }

   xbuf_xcat(reply, "</table><p>%U microseconds</p>", getus() - start);
   return HTTP_200_OK;
}
// ---------------------------------------------------------------------------
// End of Source Code
// ---------------------------------------------------------------------------

