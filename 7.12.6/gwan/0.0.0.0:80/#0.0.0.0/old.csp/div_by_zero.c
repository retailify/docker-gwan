// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// div_by_zero.c: checking what happens with such a program error (see below):
//
// Script: div_by_zero.c
// Client: 192.168.200.80
// Query : csp?div_by_zero.c
// 
// Signal        : 8:Integer divide by zero
// Signal src    : 1:SEGV_MAPERR
// errno         : 0
// Code   Pointer: 032f90c8
// Access Address: 032f90c8
// 
// Registers     : EAX=00000064 CS=0033 EIP=032f90c8 EFLGS=00010246
//                 EBX=1290e500 SS=1f269fd ESP=1290e080 EBP=1290e0b0
//                 ECX=1290e590 DS=1f269fd ESI=1290e590 FS=0033
//                 EDX=00000000 ES=1f269fd EDI=fffffffe CS=0033
// 
// Call chain    :(line) PgrmCntr(EIP)  RetAddress  FramePtr(EBP)  StackPtr(ESP)
//           main:    29     032f90c8    00441a85       1290e0b0       1290e080
// ============================================================================
#include "gwan.h" // G-WAN exported functions

int main(int argc, char *argv[])
{
   // 'j' is needed to avoid a compiler warming for the too obvious (100 / 0)
   int j = 0, i = 100 / (1 - (j + 1));
 
   // use 'i' to avoid compiler warning (unusued variable 'i')
   xbuf_xcat(get_reply(argv), "i (%d) did not crash?", i); 

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================

