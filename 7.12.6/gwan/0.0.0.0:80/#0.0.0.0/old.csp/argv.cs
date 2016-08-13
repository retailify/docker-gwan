// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// hello.cs: list all the arguments of a .NET C# servlet
// ============================================================================
using System;

public class hello
{
   static int Main(string[] args)
   {
      long reply = Gwan.getReply(args[0]);
      int nb_args = args.Length != 0 ? args.Length - 1 : 0;
      
      // list all program arguments
      String str = "<h3>main()'s argv[0-" + nb_args + "] listed:</h3>";
      for(int i = 1; i < args.Length; i++)
          str += "argv[" + i + "] '" + args[i] + "'<br>";

      Gwan.xbufCat(reply, str);
     
      return 200; // HTTP status (200:'OK')
   }
}
// ============================================================================
// End of Source Code
// ============================================================================
