// ============================================================================
// Java servlet example for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// hello.java: G-WAN supports blazing-fast Java scripts!
// ----------------------------------------------------------------------------
import api.Gwan;

public class hello // the class name MUST match the script *.java filename
{
   public static int jmain(long env, String[] args) 
   {
      Gwan.xbufCat(Gwan.getReply(env), "Hello, Java!");
      return 200;
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

