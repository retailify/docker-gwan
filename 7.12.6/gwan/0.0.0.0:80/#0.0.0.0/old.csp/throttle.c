// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// throttle.c: how to shape bandwidth usage
//
// Setup/update the send() throttling policy with a KiB/second transfer rate. 
// This can be done both by servlets and handlers.
//
// 'kbps1' is the first packets of a reply, 'kbps2' is the speed to use after
// the first KiB of the defined 'kbps1' have been sent.
//
// Throttling can be defined on a per-connection basis ('global' == FALSE)
// or globally ('global' == TRUE).
// If you are enabling the 'global' flag then the init() function of a 
// listener's connection handler is a good place to modify the default 
// Throttling policy (none) one time for all (but this global setting 
// can also be modified dynamically by handlers and servlets).
//
// Use 0 to disable throttling (if throttling was previously enabled).
//
// Examples:
// throttle_reply(argv, 100, 150, TRUE);
// throttle_reply(argv,   0,   0, TRUE);
// ============================================================================
#include "gwan.h" // G-WAN definitions
   
#define SPEED 10   // in KB/s
   
int main(int argc, char *argv[])
{
   static char msg[] = "Throttling Enabled @ %u KB/s!";
   
   xbuf_xcat(get_reply(argv), msg, SPEED);
   
   throttle_reply(argv, SPEED, SPEED, 1); // 1: global setting, 0:this connection
   
   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================

