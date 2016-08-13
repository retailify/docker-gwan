// ============================================================================
// Java servlet example for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// report.java: getting a full server report of G-WAN's state
// ----------------------------------------------------------------------------
import api.Gwan;

public class report // the class name MUST match the script *.java filename
{
   public static int jmain(long env, String[] args) 
   {
      Gwan.report(Gwan.getReply(env), 1);
      return 200;
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

