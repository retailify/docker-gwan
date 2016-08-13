// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// servlet_name.c: return the current servlet name and client IP address
// ============================================================================
#include "gwan.h" // G-WAN exported functions

int main(int argc, char *argv[])
{ 
   xbuf_xcat(get_reply(argv), 
             "<br>"
             "<b>Servlet name</b>:<br>'%s'<br><br>"
             "<b>Client address</b>:<br>'%s'<br>",
             argv[-2],
             argv[-1]);

   return HTTP_200_OK;
}
// ============================================================================
// End of Source Code
// ============================================================================
