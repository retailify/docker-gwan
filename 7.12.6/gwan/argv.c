// ============================================================================
// C source code example for the G-WAN Web Application Server (http://gwan.ch/)
// ----------------------------------------------------------------------------
// G-WAN can also run general-purpose programs (not related to the gwan server)
//
// argv.c: invoked as: ./gwan -r argv.c a b c
//
// Do not name these scripts 'main.c' to avoid conflicts with the maintenance
// script located in gwan's executable directory.
// ----------------------------------------------------------------------------
#include <stdio.h>

int main(int argc, char *argv[])
{
   printf("main()'s argv[0-%d] listed:\n\n", 
          argc ? argc - 1 : 0);

   int i = 0;
   while(i <= argc)
   {
      printf("argv[%u] '%s'\n", i, argv[i]);
      i++;
   }

   puts(" ");
   return 0;
}
// ============================================================================
// End of Source Code
// ============================================================================
