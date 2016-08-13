// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// factal.c: generate (and serve) an in-memory Mandelbrot GIF image
//
// This sample illustrates how to:
//
//    - create a raw bitmap and draw into it;
//
//    - build smooth multi-gradient color palettes for a raw bitmap;
//
//    - check the 'reply' x_buffer size to see if it matches our needs;
//
//    - build an in-memory GIF image from a raw bitmap (can be saved on disk);
//
//    - EITHER
//      make custom HTTP headers BEFORE we know the 'Content-Length' to
//      append the response body (without doing any data copy),
//      OR 
//      specify a MIME type for G-WAN to build the headers automatically.
//
// This example just shows how to generate a dynamic GIF (that can be different
// for each client).
//
// Libraries are useful when processing takes a lot of time (like for the more
// complex JPEG or PNG file formats) but if you need to serve relatively small
// images like Charts or Captchas then nothing beats GIF (in size or in speed)
// and G-WAN generates GIFs faster than dedicated graphic libraries like GD.
//
// Note how dr_gradient() makes a 256-colors image look like a true-color one.
//
// For a 400 x 400 GIF, the timing (see the Terminal where G-WAN runs) is:
//
//   Time to generate the Mandelbrot set: 16.98 ms
//   Time to generate the GIF & palette : 1.61 ms
// ============================================================================
#include "gwan.h" // G-WAN exported functions

#include <malloc.h>
#include <string.h>
// ----------------------------------------------------------------------------
// render the Mandelbrot set in the supplied bitmap
// ----------------------------------------------------------------------------
void fractals(u8 *bitmap, int width, int height, int nbrcolors)
{
   // nbrcolors is a power of two, we decrement it to later crop with '&'
   nbrcolors--, height /= 2, width /= 2;
   
   u8   *p = bitmap, *q;
   int   x, y = -height, i;
   float X, Y, zr, zi, temp, zr2, zi2;

   while(y++ < height)
   {
      x  = width;
      q  = p + x;
      Y  = ((float)y / (float)height) - 0.5;
      while(x--)
      {
         X  = (float)x / (float)width;
         zi = 0.;
         zr = 0.;
         i  = 31;

         while(i--)
         {
            temp = zr * zi;
            zr2  = zr * zr;
            zi2  = zi * zi;
            zr   = zr2  - zi2  + Y;
            zi   = temp + temp + X;
            if(zi2 + zr2 > 2.8)
               break;
         }

         // to better render palettes < 256, we should change the << 3 factor
         // (the '& nbrcolors' crop is there either for palettes < 256 colors
         // or for 'i > 32' - with this version of the code, this is useless)
         *p++ = *(q+x) = (i << 3) & nbrcolors; // using the set symmetry here
      }
      p += width;
   }
}
// ----------------------------------------------------------------------------
// a stripped-down* version of itoa() - [*]: no checks, no ending-null
// ----------------------------------------------------------------------------
inline void u32toa(char *p, u32 v)
{
   do *p-- = '0' + (v % 10), v /= 10; while(v);
}
// ----------------------------------------------------------------------------
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//       getus(): get current time in microseconds (1 millisecond = 1,000 us)
//     get_env(): get connection's 'environment' variables from the server
//    xbuf_cat(): like strcat(), but it works in the specified dynamic buffer 
//   gif_build(): build an in-memory GIF image from a bitmap and palette
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // -------------------------------------------------------------------------
   // get a pointer on the server reply
   // -------------------------------------------------------------------------
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // allocate memory for a raw bitmap
   // -------------------------------------------------------------------------
   int   w = 800, h = 800, nbcolors = 256, wXh = w * h;
   u8 *bmp = (u8*)malloc(wXh);
   if(!bmp) return HTTP_503_SERVICE_UNAVAILABLE;

   // -------------------------------------------------------------------------
   // render the Mandelbrot set in our bitmap
   // -------------------------------------------------------------------------
   fractals(bmp, w, h, nbcolors);

   // -------------------------------------------------------------------------
   // display the palette (useful when playing with 'tabcol[]' values)
   // -------------------------------------------------------------------------
   {  
      #define ROUND(a) ((a) > 0 ? (int)((a)+0.5) : -(int)(0.5-(a)))
      u8 *p = bmp;
      int i = h, wd20 = w / 20;
      float color = 0, col = (float)nbcolors / (float)h;
      while(i--)
      {
         color += col;
         memset(p, ROUND(color) & 255, wd20);
         p += w;
      }   
   }

   // -------------------------------------------------------------------------
   // build a smooth multi-gradient color palette from the fixed values below
   // -------------------------------------------------------------------------
   static 
   rgb_t tabcol[]={ {  0,   0, 128}, // Med. Blue
                    {  0, 100, 200}, // Light Blue
                    {100, 160, 160}, // Cyan
                    {  0, 220, 100}, // Green
                    {255, 255,   0}, // Yellow
                    {255, 128,   0}, // Orange
                    {128,   0,   0}, // Med. Red
                    { 64,   0,   0}, // Dark Red
                    {128,   0,   0}, // Med. Red
                    {255, 128,   0}, // Orange
                    {255, 255,   0}, // Yellow
                    {  0, 220, 100}, // Green 
                    {100, 160, 160}, // Cyan
                    {  0, 100, 200}, // Light Blue
                    {  0,   0, 128}, // Med. Blue

                    { 64,   0,   0}, // Dark Red
                    {128,   0,   0}, // Med. Red
                    {255, 128,   0}, // Orange
                    {255, 255,   0}, // Yellow
                    {  0, 220, 100}, // Green 
                    {100, 160, 160}, // Cyan
                    {  0, 100, 200}, // Light Blue
                    {  0,   0, 128}, // Med.  Blue
                    {  0,   0,  64}, // Dark  Blue
                  }, *tab = tabcol;
                  
   // -------------------------------------------------------------------------
   // just for fun, select different colors for each call
   // -------------------------------------------------------------------------
   static u32 call = 0;
   u32 ncols = sizeof(tabcol) / sizeof(rgb_t);
   switch(call)
   {
      case 0: tab =  tabcol;     ncols = 10; break; // blue
      case 1: tab = &tabcol[ 4]; ncols = 10; break; // yellow
      case 2: tab = &tabcol[ 7]; ncols =  7; break; // dark red
      case 3: tab = &tabcol[ 1]; ncols = 16; break; // rainbow -
   }
   call = (call + 1) & 3;
   // generate the palette with our defined gradient steps
   u8 pal[768]; dr_gradient(pal, nbcolors, tab, ncols);
   // nice palete but we want a black body to delimit the mandelbrot set
   memset(pal + ((nbcolors - (nbcolors / 16)) * 3), 0, (nbcolors / 16) * 3);

   // -------------------------------------------------------------------------
   // create custom HTTP response headers to send a GIF file
   // -------------------------------------------------------------------------
   // (G-WAN automatically generates headers if none are provided but it can't
   //  guess all MIME types so this automatic feature is for 'text/html' only
   //  ...unless you explicitly specify the reply buffer MIME type)

#ifdef BUILD_CUSTOM_HEADERS // old way of doing things

   // get the current HTTP date (like "Wed, 02 Jun 2010 06:49:37 GMT")
   u8 *date = (u8*)get_env(argv, SERVER_DATE);

   xbuf_xcat(reply,
             "HTTP/1.1 200 OK\r\n"
             "Date: %s\r\n"
             "Last-Modified: %s\r\n"
             "Content-type: image/gif\r\n"
             "Content-Length:       \r\n" // make room for the for GIF length
             "Connection: close\r\n\r\n",
             date, date);

   // -------------------------------------------------------------------------
   // make sure that we have enough space in the 'reply' buffer
   // (we are going to fill it directly from gif_build(), not via xbuf_xxx)
   // -------------------------------------------------------------------------
   // (if we have not enough memory, we will get a 'graceful' crash)
   if(reply->allocated < (wXh / 10)) // very gross approximation
   {
      if(!xbuf_growto(reply, wXh / 10)) // resize reply
      {
         xbuf_init(reply);
         xbuf_ncat(reply, " ", 1);
         reply->len = 0; // discart pointless data, keep allocated memory
         return HTTP_503_SERVICE_UNAVAILABLE; // not enough memory
      }
   }

   // -------------------------------------------------------------------------
   // save the place where to patch the void 'Content-Length' HTTP Header
   // -------------------------------------------------------------------------
   char *p = reply->ptr + reply->len
         - (sizeof("\r\nConnection: close\r\n\r\n") - 1);

   // -------------------------------------------------------------------------
   // append a GIF image (-1:no transparency, 0: no comment) to 'reply'
   // -------------------------------------------------------------------------
   int len = gif_build((u8*)(reply->ptr + reply->len), bmp, w, h, 
                       pal, nbcolors, 
                       -1, 0);
   if(len < 0) len = 0; // (len == -1) if gif_build() failed
   reply->len += len;  // add the GIF size to the 'reply' buffer length
   free(bmp);
   
   // -------------------------------------------------------------------------
   // store the GIF size in the empty space of the 'Content-Length' header
   // -------------------------------------------------------------------------
   u32toa(p, len);
   
#else // #ifdef BUILD_CUSTOM_HEADERS // works with any supported MIME type
   
   // -------------------------------------------------------------------------
   // specify a MIME type so we don't have to build custom HTTP headers
   // -------------------------------------------------------------------------
   char *mime = (char*)get_env(argv, REPLY_MIME_TYPE);
   // note that we setup the FILE EXTENSION, not the MIME type:
   mime[0] = '.'; mime[1] = 'g'; mime[2] = 'i'; mime[3] = 'f'; mime[4] = 0; 

   // -------------------------------------------------------------------------
   // make sure that we have enough space in the 'reply' buffer
   // (we are going to fill it directly from gif_build(), not via xbuf_xxx)
   // -------------------------------------------------------------------------
   // (if we have not enough memory, we will get a 'graceful' crash)
   if(reply->allocated < (wXh / 10)) // very gross approximation
   {
      if(!xbuf_growto(reply, wXh / 10)) // resize reply
      {
         xbuf_init(reply);
         xbuf_ncat(reply, " ", 1);
         reply->len = 0; // discart pointless data, keep allocated memory
         return HTTP_503_SERVICE_UNAVAILABLE;
      }
   }

   // -------------------------------------------------------------------------
   // append a GIF image (-1:no transparency, 0: no comment) to 'reply'
   // -------------------------------------------------------------------------
   int len = gif_build((u8*)(reply->ptr + reply->len), bmp, w, h, 
                       pal, nbcolors, 
                       -1, 0);
   if(len < 0) len = 0; // (len == -1) if gif_build() failed
   reply->len += len;  // add the GIF size to the 'reply' buffer length
   free(bmp);
   
#endif // #else #ifdef BUILD_CUSTOM_HEADERS

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
