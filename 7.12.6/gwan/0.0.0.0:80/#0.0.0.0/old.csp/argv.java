// ============================================================================
// Java servlet example for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// argv.java: how to list *all* URL parameters (may useful in RESTFUL services)
//            without gwan_get_arg() like in the 'loan.java' example.
//
// With /?argv.java&name=Eva&amount=1000000&rate=3.5&term=1, the output is:
//
//   main()'s argv[0-3] listed:
//   argv[0] 'name=Eva'
//   argv[1] 'amount=1000000'
//   argv[2] 'rate=3.5'
//   argv[3] 'term=1'
// ----------------------------------------------------------------------------
import api.Gwan;

public class argv // the class name MUST match the script *.java filename
{
   public static int jmain(long env, String[] args) 
   {
      int nb_args = args.length != 0 ? args.length - 1 : 0;
      String str = "<h3>main()'s argv[0-" + nb_args + "] listed:</h3>";
      int i = 0;
      for(String s: args)
      {
          str += "argv[" + i + "] '" + s + "'<br>";
          i++;
      }
     
      long reply = Gwan.getReply(env);
      Gwan.xbufCat(reply, str);
      return 200;
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

