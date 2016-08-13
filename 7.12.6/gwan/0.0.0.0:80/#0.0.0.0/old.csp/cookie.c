// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// set_cookie.c: set a cookie and visualize it with /csp/served_from.c
//
// Note: EU Cookie Law
//       Directive 2009/136/EC Article 5 Paragraph 3 requires user consent to
//       obtained for cookie storage (its goal is to reduce user tracking).
//       Cookies are exempted from informed consent if they are used for user
//       input when filling online forms, as a shopping cart or as mere tokens
//       for first-level (origin server) authentication.
// ============================================================================
#include "gwan.h" // G-WAN exported functions

int main(int argc, char *argv[])
{
   // "Set-Cookie: Domain=.foo.com; Path=/; Max-Age=%u\r\n"
   const char cookie[] = "Set-Cookie: Max-Age=3600\r\n" // 1 hour
                         "Location: /?served_from\r\n\r\nblah\r\n";
   http_header(HEAD_ADD, (char*)cookie, sizeof(cookie) - 1, argv);

   return HTTP_301_MOVED_PERMANENTLY; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================

