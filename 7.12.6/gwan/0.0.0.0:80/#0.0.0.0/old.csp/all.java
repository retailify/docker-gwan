// ============================================================================
// Java servlet example for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// all.java: illustrating all the (Java) G-WAN API
// ----------------------------------------------------------------------------
import api.Gwan;

public class all // the class name MUST match the script *.java filename
{
   public static int jmain(long env, String[] args) 
   {
      double a = 123.456;
      String str = "<h3>G-WAN API</h3><br><table>"
    + String.format("<tr><td>Gwan.cycles64() =</td><td>%,d</td></tr>", Gwan.cycles64())
    + String.format("<tr><td>Gwan.getNs() =</td><td>%,d</td></tr>", Gwan.getNs())
    + String.format("<tr><td>Gwan.getUs() =</td><td>%,d</td></tr>", Gwan.getUs())
    + String.format("<tr><td>Gwan.getMs() =</td><td>%,d</td></tr>", Gwan.getMs())
    + "</table><br><br>"
    + String.format("double a = '%,.2f'<br>", a);
      
      Gwan.xbufCat(Gwan.getReply(env), str);
      return 200;
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

