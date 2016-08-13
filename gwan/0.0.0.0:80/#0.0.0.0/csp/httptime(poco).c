// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// httptime.c: Poco vs. G-WAN comparative benchmark
//
// Someone asked what advantage G-WAN has vs the Poco, ACE or BOOST libraries.
//
// The Poco framework boasts that it has more than 8,880 classes (each having
// several functions). The PHP madness (6,000+ functions) is clearly beaten.
//
// "Ease of use" and "low learning curve", hmm...
//
// Then, it took 25 minutes for make to BUILD Poco on a 6-Core 3.46 GHz PC 
// with 8 GB of RAM (nice "reloading time" when you edit the source code...).
//
// On the same machine, I tested the *236 lines of C++* Poco HTTPTimeServer 
// sample (a nice example of what the C++ nonsense can do to turn a simple 
// problem into a complexity abomination).
//
// By comparison, the equivalent C code for G-WAN takes a mere 16 lines (yes,
// this is 15 times less than 236 lines, see the source code below).
//
// OK, so Poco's value clearly DOES NOT reside into simplicity, ease of use 
// nor code conciseness...
//
// But, probably, Poco's value is elsewhere, in extraordinary inner mechanisms 
// built by rocket scientists to deliver the kind of high-performances or 
// high-scalability that nobody has ever seen elsewhere, right?
//
// The Poco HTTPTimeServer sample (which does not support HTTP parsing nor 
// HTTP Keep-Alives, and is serving only a date string) gives:
//
//   -------------------------------------------
//   ab -n 1000000 -c 100 -d -t 1 "http://127.0.0.1:9980/"
//   -------------------------------------------
//   Document Length:        233 bytes
//   Concurrency Level:      100
//   Requests per second:    10,091.14 [#/sec] (mean)
//   ------------------------=========----------
//
//   BOOST and ACE are even larger and slower, I have been told.
//   
// G-WAN serves the same Date string MORE THAN 15 TIMES FASTER (despite 
// serving MORE data with more HTTP headers built on-the-fly, like ETags, 
// Expire, etc.):
//
//   -------------------------------------------
//   ab -n 1000000 -c 100 -d -k -t 1 "http://127.0.0.1:9980/?httptime.c"
//   ===========================================
//   Document Length:        262 bytes
//   Concurrency Level:      100
//   Requests per second:    159,951.89 [#/sec] (mean)
//   ========================**********=========
// 
// And when limited by the no-Keep-Alives bottleneck, G-WAN is still faster
// than Poco with HTTP keep-Alives:
//
//   -------------------------------------------
//   ab -n 1000000 -c 100 -d -t 1 "http://127.0.0.1:9980/?httptime.c"
//   -------------------------------------------
//   Document Length:        262 bytes (I just re-used the httpdate.c sample)
//   Concurrency Level:      100
//   Requests per second:    24,235.51 [#/sec] (mean)
//   ------------------------=========----------
//
// And this is merely a single-thread test with AB (ApacheBench). Use weighttp
// with "-t 4" to use a client worker thread pre CPU Core and the Poco gap will 
// be 4 times higher!
//
// May I recall you that G-WAN uses C scripts that are executed on-the-fly 
// while the Poco sample is pre-compiled and linked with all the C compiler
// optimization options enabled?
// 
// Once you made a change to a server application, with Poco, ACE, BOOST or
// Apache/Nginx modules, you have to do:
// 
//   - a server stop (at least for Nginx),
//   - a recompilation, liking, 
//   - a server re-configuration,
//   - a server restart.
//
// I did not feel necessary to measure the CPU usage but this would be fun too.
//
// So, G-WAN is the clear winner at developing Web applications, on simplicity, 
// efficiency and reliability (no security breach -unlike all others...).
//
// Why make it simple when you can make it so beautifully complex, error-prone 
// and inefficient?
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//    time2rfc(): generate an RFC Date/Time stamp, for the given EPOCH time
//    xbuf_cat(): like strcat(), but it works in the specified dynamic buffer 
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN definitions

static char top[] = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
   "<html lang=\"en\"><head><title>HTTP Time</title><meta http-equiv"
   "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
   "</head><body style=\"background:#fff;\"><h1>HTTP Time</h1>%s"
   "</body></html>";

int main(int argc, char *argv[])
{
   char date[32]; time2rfc(0, date);

   xbuf_xcat(get_reply(argv), top, date);

   return HTTP_200_OK;
}
// ============================================================================
// End of Source Code
// ============================================================================

