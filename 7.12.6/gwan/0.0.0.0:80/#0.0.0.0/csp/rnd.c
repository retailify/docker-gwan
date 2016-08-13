// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// rnd.c: produced a (true) random file of the specified size
// ============================================================================
#include "gwan.h" // G-WAN exported functions

#include <stdlib.h> // atol()
#include <stdio.h>  // puts()

int main(int argc, char *argv[])
{
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // check if we were passed a 'size' as an URL parameter
   // -------------------------------------------------------------------------
   char *Size = 0;
   get_arg("size=", &Size, argc, argv);
   if(!Size || !*Size)
   {
usage:   
      xbuf_cat(reply, "usage: http://127.0.0.1:8080/?rnd&size=1000 (in bytes)");
      return HTTP_200_OK;
   }

   // -------------------------------------------------------------------------
   // check if 'size' contains a 'K', or 'M' to get Kilobytes or Megabytes
   // -------------------------------------------------------------------------
   //printf("parsing... (%s)\n", Size);
   int size = 0;
   {
      char *p = Size;
      for(; *p && size < 2 * 1024 * 1024; p++) // limit size to 2 MB
      {
         switch(*p)
         {
            case '0' ... '9': size = (10 * size) + (*p - '0'); break;
            case         'K':
            case         'k': size <<= 10; break;
            case         'M':
            case         'm': size <<= 20; break;
            default         : break;
         }
      }
   }
   if(!size)
   {
      xbuf_cat(reply, "Can't generate a 0-byte file!<br><br>");
      goto usage;
   }
   
   xbuf_xcat(reply, "Generating a %k%s RND file...",
             size,
             size < 1024 ? "-byte" : "");

   // -------------------------------------------------------------------------
   // create a buffer to receive the random data
   // -------------------------------------------------------------------------
   xbuf_t buf;
   xbuf_init(&buf);
   xbuf_growto(&buf, size);

   // -------------------------------------------------------------------------
   // generate random data
   // -------------------------------------------------------------------------
   //printf("generating %d-byte rnd data...\n", size);
   {
      buf.len = size;
      u32 *p = (u32*)buf.ptr;
      rnd_t rnd;
      hw_init(&rnd);
      int i = 1 + size / sizeof(u32);
      while(i--)
         *p++ = hw_rand(&rnd);
   }

   // -------------------------------------------------------------------------
   // save buffer in a file located under our /gwan/.../www directory
   // -------------------------------------------------------------------------
   //puts("saving file...");
   char *www_root = (char*)get_env(argv, WWW_ROOT);
   char filename[512];
   s_snprintf(filename, sizeof(filename)-1, "%s/%k.bin", www_root, size);
   xbuf_tofile(&buf, filename);
   xbuf_free  (&buf);

   xbuf_cat(reply, " done.");
   return HTTP_200_OK;
}
// ============================================================================
// End of Source Code
// ============================================================================

