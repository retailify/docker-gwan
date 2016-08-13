// ============================================================================
// Java API calls for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// Compile with: javac Gwan.java
// 
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
package api;

public class Gwan
{
   public static native long   getReply (long env);
   public static native void   xbufCat  (long ctx, String str);
   public static native long   cycles64 ();
   public static native long   getIntEnv(long env, int name);
   public static native String getStrEnv(long env, int name);
   public static native long   getNs    ();
   public static native long   getUs    ();
   public static native long   getMs    ();
   public static native void   logErr   (long env, String msg);
   public static native void   report   (long reply, int html_format);
   
   // -------------------------------------------------------------------------
   // getEnv() server 'environment' variables (Java enums {} are a pain)
   // -------------------------------------------------------------------------
   public static class http_env
   {
      public static final int REQUEST           = 0;
      public static final int REQUEST_LEN       = 1;
      public static final int REQUEST_METHOD    = 2;
      public static final int QUERY_STRING      = 3;
      public static final int FRAGMENT_ID       = 4;
      public static final int REQ_ENTITY        = 5;
      public static final int CONTENT_TYPE      = 6;
      public static final int CONTENT_LENGTH    = 7;
      public static final int CONTENT_ENCODING  = 8;
      public static final int SESSION_ID        = 9;
      public static final int HTTP_CODE         = 10;
      public static final int HTTP_HEADERS      = 11;
      public static final int AUTH_TYPE         = 12;
      public static final int REMOTE_ADDR       = 13;
      public static final int REMOTE_BIN_ADDR   = 14;
      public static final int REMOTE_PORT       = 15;
      public static final int REMOTE_PROTOCOL   = 16;
      public static final int REMOTE_USER       = 17;  
      public static final int REMOTE_PWD        = 18;
      public static final int CLIENT_SOCKET     = 19;
      public static final int USER_AGENT        = 20;
      public static final int SERVER_SOFTWARE   = 21;
      public static final int SERVER_NAME       = 22;
      public static final int SERVER_ADDR       = 23;
      public static final int SERVER_PORT       = 24;
      public static final int SERVER_DATE       = 25;
      public static final int SERVER_PROTOCOL   = 26;
      public static final int VHOST_ROOT        = 27;
      public static final int WWW_ROOT          = 28;
      public static final int CSP_ROOT          = 29;
      public static final int LOG_ROOT          = 30;
      public static final int HLD_ROOT          = 31;
      public static final int FNT_ROOT          = 32;
      public static final int DOWNLOAD_SPEED    = 33;
      public static final int MIN_READ_RATE     = 33; // alias, not a mistake
      public static final int READ_XBUF         = 34;
      public static final int SCRIPT_TMO        = 35;
      public static final int KALIVE_TMO        = 36;
      public static final int REQUEST_TMO       = 37;
      public static final int MIN_READ_SPEED    = 38;
      public static final int NBR_CPUS          = 39;
      public static final int NBR_CORES         = 40;
      public static final int NBR_WORKERS       = 41;
      public static final int CUR_WORKER        = 42;
      public static final int REPLY_MIME_TYPE   = 43;
      public static final int DEFAULT_LANG      = 44;
      public static final int QUERY_CHAR        = 45;
      public static final int REQUEST_TIME      = 46;
      public static final int MAX_ENTITY_SIZE   = 47;
      // -------------------------------------------------------------------------
      // Server performance counters
      // -------------------------------------------------------------------------
      public static final int CC_BYTES_IN       = 100;
      public static final int CC_BYTES_OUT      = 101;
      public static final int CC_ACCEPTED       = 102;
      public static final int CC_CLOSED         = 103;
      public static final int CC_REQUESTS       = 104;
      public static final int CC_HTTP_REQ       = 105;
      public static final int CC_CACHE_MISS     = 106;
      public static final int CC_ACPT_TMO       = 107;
      public static final int CC_READ_TMO       = 108;
      public static final int CC_SLOW_TMO       = 109;
      public static final int CC_SEND_TMO       = 110;
      public static final int CC_BUILD_TMO      = 111;
      public static final int CC_CLOSE_TMO      = 112;
      public static final int CC_CSP_REQ        = 113;
      public static final int CC_STAT_REQ       = 114;
      public static final int CC_HTTP_ERR       = 115;
      public static final int CC_EXCEPTIONS     = 116;
      public static final int CC_BYTES_INDAY    = 117;
      public static final int CC_BYTES_OUTDAY   = 118;
      // -------------------------------------------------------------------------
      // Handler and VirtualHost Persistence pointers
      // -------------------------------------------------------------------------
      public static final int US_REQUEST_DATA     = 200;
      public static final int US_HANDLER_DATA     = 201;
      public static final int US_VHOST_DATA       = 202;
      public static final int US_SERVER_DATA      = 203;
      public static final int US_HANDLER_STATES   = 204;   
      public static final int US_HANDLER_CTX_SIZE = 205; 
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

