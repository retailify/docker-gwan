// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// hello.cs: G-WAN supports .NET C# servlets
// ============================================================================
using System;

public class hello
{
   public static int Main(string[] args)
   {
      Gwan.xbufCat(Gwan.getReply(args[0]), "Hello, C#!");
      return 200; // HTTP status (200:'OK')
   }
}
// ============================================================================
// End of Source Code
// ============================================================================
