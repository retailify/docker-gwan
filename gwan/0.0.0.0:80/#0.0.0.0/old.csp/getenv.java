// ============================================================================
// Java servlet example for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// get_env.java: how to access G-WAN envorinment variables from Java & Scala
// ----------------------------------------------------------------------------
// The G-WAN API getStrEnv() Java function returns a Java string for:
//
//   REQUEST             +
//   QUERY_STRING        |
//   FRAGMENT_ID         |
//   REQ_ENTITY          |
//   VHOST_ROOT          |
//   WWW_ROOT            |
//   CSP_ROOT            |
//   LOG_ROOT            | a copy of the G-WAN C string
//   HLD_ROOT            | 
//   FNT_ROOT            |
//   REMOTE_ADDR         |
//   USER_AGENT          |
//   SERVER_SOFTWARE     |
//   SERVER_NAME         |
//   SERVER_ADDR         |
//   SERVER_DATE         +
//
// Use getIntEnv() for the many integer values (like REQUEST_LEN)
//
// ...but these get_env() values are ignored by Java getxxxEnv() calls:
//
//   HTTP_CODE           +
//   SCRIPT_TMO          |
//   KALIVE_TMO          |
//   REQUEST_TMO         | 
//   DOWNLOAD_SPEED      | pointers on integers
//   MIN_READ_SPEED      |
//   MAX_ENTITY_SIZE     |
//   US_HANDLER_STATES   +
//
//   US_HANDLER_DATA     +
//   US_VHOST_DATA       | pointers on user-defined data structures
//   US_SERVER_DATA      +
//
//   READ_XBUF           + pointers on G-WAN internal structures
//   HTTP_HEADERS        +
//
// If you want to contribute in this matter (by helping to implement Java 
// G-WAN API in C with JNI), then drop us a line, you are most welcome!
// ----------------------------------------------------------------------------
import api.Gwan;

public class getenv // the class name MUST match the script *.java filename
{
   public static int jmain(long env, String[] args) 
   {
      // get Integer and String G-WAN SERVER HTTP variables
      long req_len = Gwan.getIntEnv(env, Gwan.http_env.REQUEST_LEN);
      String req   = Gwan.getStrEnv(env, Gwan.http_env.REQUEST);

      // build the server response UTF8 string
      String result = "Request Length: " + req_len + "<br>'" + req + "'<br>";
      
      // give response to G-WAN
      Gwan.xbufCat(Gwan.getReply(env), result);
      
      return 200; // 200:'OK' HTTP server status code
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

