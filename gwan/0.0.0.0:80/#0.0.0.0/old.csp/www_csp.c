// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// www_csp.c: how to get the csp and www path of the current Web site
// ============================================================================
#include "gwan.h"

int main(int argc, char *argv[])
{
   // the path can be used to save or load a data file (request.c, contact.c)
   char *www = (char*)get_env(argv, WWW_ROOT);
   char *csp = (char*)get_env(argv, CSP_ROOT);

   xbuf_xcat(get_reply(argv), "%s<br />\n%s<br />\n", www, csp);
   return HTTP_200_OK;
}
// ============================================================================
// End of Source Code
// ============================================================================

