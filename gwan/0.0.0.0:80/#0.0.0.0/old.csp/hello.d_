// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// hello.d: just used with Lighty's Weighttp to benchmark a minimalist servlet
//
// Note: the D language specs and implementation change all the time, breaking 
//       the  compatibility with prior versions. Below there's the D string vs
//       C char* casting behavior illustrated, but the D language bindings and
//       runtime compilation model have also been modified making it difficult
//       for developers to stay in touch with D.
//
//       I got it to work again but you will have to recompile the D runtime
//       /usr/lib/gcc/x86_64-linux-gnu/4.6/libgphobos2.a with -fPIC to comply
//       with the latest D requirements.
// ============================================================================
extern (C) // could a D developer help to translate the gwan.h prototypes?
{
   void *get_reply(char[][]);
   char *xbuf_ncat(void *, char *, uint);
}

import std.string; // required now to pass D strings to C

int main(char [][]argv)
{
   xbuf_ncat(get_reply(argv), 
   
             // that's the previously working syntax:
             //"Hello World (D)", 
   
             // that's the new (more intuitive?) syntax:
             cast(char*)toStringz("Hello World (D)"), 
   
             "Hello World (C)".sizeof - 1);
             
   return 200;
}
// ============================================================================
// End of Source Code
// ============================================================================

