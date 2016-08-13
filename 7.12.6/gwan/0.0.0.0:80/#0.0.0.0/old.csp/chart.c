// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// chart.c: generate (and serve) in-memory area/bar/dot/line/pie/ring Charts
//
// This sample illustrates how to:
//
//    - create a raw bitmap and draw a CHART (like Yahoo! Financial charts);
//
//    - use fonts (TrueType, PSF, GIF, see below) to print TEXT in a bitmap;
//
//    - use dr_circle() and dr_line() to draw in the bitmap;

//    - build an in-memory GIF image from a raw bitmap (can be saved on disk);
//
//    - make custom HTTP headers BEFORE we know the actual 'Content-Length' to
//      append the response body (without doing any data copy).
//
// This GIF Bar Chart is 4x smaller than the original PNG image from Yahoo!
// and generating small GIF images is much faster than generating PNG images.
//
// A 192 x 96 Line+Area+Average+Titles+FilledGrid+Labels Chart takes:
//
//   Time to generate the Chart: 0.08 ms
//   Time to generate the GIF  : 0.28 ms
//
// That's much faster than all the charting libraries that I have seen. Don't 
// get my word for it and do your own benchmarks. Performance is a 'feature'.
// ----------------------------------------------------------------------------
// TrueType Fonts
// ----------------------------------------------------------------------------
//   To use TrueType fonts, type the characters* in OpenOffice, make a screen
//   capture and crop it to save only the text in a GIF file. That's all, you
//   can now call G-WAN's dr_text() with the GIF file name (if the full-path 
//   isn't supplied then G-WAN will search the font file under gwan/fonts).
//
//   [*]: !"#$%&'{}*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
//        abcdefghijklmnopqrstuvwxyz{|}~
//
//   All characters must be separated by at least a blank pixel, see the
//   default font '9pts.gif' used by this example, you can adjust this by
//   using OpenOffice's "Format" / "Characters" / "Position" / "Spacing".
//   You may also need to disable "Subpixel smoothing" in the system 
//   "Appearance Preferences" / "Fonts" GNOME configuration panel to
//   get a 'clean' GIF font (restore this option after the capture).
//
//   To add TrueType fonts under Linux Ubuntu, create a /home/.fonts folder 
//   and copy your *.ttf files here. They are now available for applications.
//   As a '.' prefixed folder is invisible under GNOME, you might have to
//   make it visible in Nautillus "Edit" / "Preferences" / "Show hidden and
//   backup files" in order to do things like copy *.ttf files in it or make
//   a shortcut on your Desktop.
// ----------------------------------------------------------------------------
// Linux PSF (PC Screen Font) console fonts
// ----------------------------------------------------------------------------
//   Linux Ubuntu 8.1 comes with 267 international PSF fonts (high-quality 
//   bitmap fonts crafted for screen rendering) in /usr/share/consolefonts.
//   G-WAN's dr_text() could also use those fonts in a future version if 
//   there is enough demand. This would also provide localized fonts.
// ============================================================================
#include "gwan.h" // G-WAN exported functions

#include <malloc.h>
#include <string.h>

#define MIN(a,b) ((a) <= (b) ? (a) : (b))

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
//     sw_rand(): a decent pseudo-random numbers generator
//     get_env(): get connection's 'environment' variables from the server
//    xbuf_cat(): like strcat(), but it works in the specified dynamic buffer 
//   gif_build(): build an in-memory GIF image from a bitmap and palette
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // -------------------------------------------------------------------------
   // the color palette used by our charts (sized to 256 for gradients)
   // -------------------------------------------------------------------------
   enum { black, dgrey, tgrey, lgrey, fgrey, 
          dblue, mblue, fblue, lblue, orange, white };

   // this is the place for you to edit chart colors:
   static rgb_t pal[256] = {
      [black]  = {  0,   0,   0},   // Title
      [dgrey]  = {128, 128, 128},   // Ticks/Labels
      [tgrey]  = {164, 164, 164},   // Sub-title
      [lgrey]  = {220, 220, 220},   // Grid lines
      [fgrey]  = {240, 240, 240},   // Grid filling
      [dblue]  = { 63, 116, 167},   // Data line
      [mblue]  = {195, 216, 230},   // Data Grid lines
      [fblue]  = {204, 225, 239},   // Data Grid filling
      [lblue]  = {211, 231, 244},   // Data area
      [orange] = {255,  75,   3},   // Data average
      [white]  = {255, 255, 255} }; // Background

   // -------------------------------------------------------------------------
   // build a smooth gradient color palette (used by pie/ring charts)
   // -------------------------------------------------------------------------
   static int firsttime = 1;
   if(firsttime) // only once
   {
      firsttime = 0;
      static rgb_t tabcol[] = { 
              {128,   0,   0},   // Red
              {255, 128,   0},   // Orange
              {255, 255,   0},   // Yellow
              {  0, 220, 100},   // Green
              {  0, 100, 200},   // Light Blue
              {  0,   0, 128} }; // Dark Blue
                  
      // generate a gradient palette from these pre-defined color steps
      dr_gradient((u8*)(pal + white + 1), 
                  32 - (white + 1), 
                  tabcol, 
                  sizeof(tabcol) / sizeof(rgb_t));
   }
   
   // -------------------------------------------------------------------------
   // make data for our chart
   // -------------------------------------------------------------------------
   char *tags[]= {"","10am","","","","12pm","","","","2pm","","","","4pm"};
   u32 ntag    = sizeof(tags) / sizeof(u8*);
   u8  date [] = "Jun 01 03:59pm EDT";
   u8  title[] = "DOW";
   // Chart data sets
   float tab[] = {10042, 10098, 10182, 10154, 10160, 10132, 10160, 10146, 
                  10215, 10134, 10152, 10122, 10116, 10030};
// float tab[] = {18, 80, 18, 54, 60, 32, 60, 46, 15, 34, 52, 22, -16, 100};
// float tab[] = {1, 8, 3, 5, 7, 1, -6, 4, 5, 3, 5, 2, -1, 1};
// float tab[] = {-1.5, .8, .3, .5, .7, .1, .6, .9, .5, .3, .5, .2, .3, .1};
   int ntab    = sizeof(tab) / sizeof(float);
   /*
   #define nvals 200  // use more values than we have pixels, forcing dr_chart()
   float vals[nvals]; // to use interpolation (see the dr_chart() call below)
   {
      prnd_t rnd;
      sw_init(&rnd, cycles());
      int i = 0, n, nb = (nvals / ntab) + 1;
      for(; i < nvals; i++)
      {
          n = sw_rand(&rnd); // use pseudo-random data to fill the gaps
          vals[i] = tab[i / nb] + (-(n & 1) * (n % ntab));
      }
   }*/
   
   // -------------------------------------------------------------------------
   // setup the Chart SIZE and STYLE and allocate memory for a raw bitmap
   // -------------------------------------------------------------------------
   int types[] = { C_LINE,
                   C_LINE | C_GRID,
                   C_LINE | C_TITLES | C_LABELS | C_GRID,
                   C_LINE | C_TITLES | C_LABELS | C_FGRID,
                   C_LINE | C_TITLES | C_LABELS | C_GRID | C_AVERAGE,
                   C_LINE | C_TITLES | C_LABELS | C_FGRID | C_AREA | C_AVERAGE,
                   C_BAR,
                   C_BAR | C_TITLES | C_GRID,
                   C_BAR | C_LABELS | C_GRID | C_AVERAGE,
                   C_BAR | C_TITLES | C_LABELS | C_FGRID | C_AREA | C_AVERAGE,
                   C_DOT | C_AREA | C_GRID,
                   C_DOT | C_TITLES | C_LABELS | C_FGRID | C_AREA | C_AVERAGE,
                   C_RING,
                   C_RING | C_AREA,
                   C_RING | C_TITLES | C_LABELS,
                   C_RING | C_TITLES | C_LABELS | C_AREA,
                   C_PIE,
                   C_PIE | C_AREA,
                   C_PIE | C_TITLES | C_LABELS,
                   C_PIE | C_TITLES | C_LABELS | C_AREA
                 };
   
   static int chart_type = 11;
   if(chart_type >= (sizeof(types) / sizeof(int)))
      chart_type = 0;
      
   bmp_t img; memset(&img, 0, sizeof(bmp_t));
   img.w     = 192; // the size of the original YAHOO! chart
   img.h     =  96;
// img.bbp   =   3; // 1 << 3 =  8 colors // too small for all pie/ring charts
   img.bbp   =   5; // 1 << 5 = 32 colors // allowing 'rainbow' charts...
   img.pen   =  11; // pie/ring: gradient starting color index (0:no gradient)
   img.flags = types[chart_type++];
   img.bmp   = (u8*)malloc(img.w * img.h);
   if(!img.bmp) return HTTP_503_SERVICE_UNAVAILABLE;

   // -------------------------------------------------------------------------
   // render the Chart in our raw bitmap
   // -------------------------------------------------------------------------
// dr_chart(&img, title, date, tags, ntag, vals, nvals); // use more values
   dr_chart(&img, title, date, (u8**)tags, ntag, tab, ntab); // no interpolation
   
   // -------------------------------------------------------------------------
   /*/ display the palette (useful when playing with 'palette[]' values)
   // -------------------------------------------------------------------------
   {  u8 *p = img.bmp;
      int i = img.h, wd20 = img.w / 20, col = ROUND(img.h / 32.);
      while(i--)
      {
         memset(p, i / col, wd20); // kind of dr_horizontal_line()
         p += img.w;
      }   
   }*/

   // -------------------------------------------------------------------------
   // highlight an area - just a pretext to show how to use dr_circle()
   // -------------------------------------------------------------------------
   if(!(img.flags & (C_PIE | C_RING))) // it messes round charts too much
   {
      img.pen = orange; // color border of the circle
      img.bgd =     -1; // circle not filled, to fill it, use a color index
      //       (&img, x,y:center,           radius)
      dr_circle(&img, img.w / 2, img.h / 2, MIN(img.w, img.h) / 4);
   }
   
   // -------------------------------------------------------------------------
   // highlight an area - just a pretext to show how to use dr_line()
   // -------------------------------------------------------------------------
   img.pen = orange; // color of the lines
   img.p   =      0; // no filling below the lines ('p' is a bitmap pointer)
   //     (&img,  x,  y,  X,  Y);
   dr_line(&img,  0, 10, 20, 10); // +-----------+
   dr_line(&img,  0, 10,  0, 14); // |
   dr_line(&img, 20, 10, 20, 14); //             |
   dr_line(&img,  0, 14, 20, 14); // +-----------+

   // -------------------------------------------------------------------------
   // get a pointer to the server reply
   // -------------------------------------------------------------------------
   xbuf_t *reply = get_reply(argv);

   // -------------------------------------------------------------------------
   // create custom HTTP response headers to send a GIF file
   // -------------------------------------------------------------------------
   // (G-WAN automatically generates headers if none are provided but it can't
   //  guess all MIME types so the automatic feature is for 'text/html' only)
   {
      // get the current HTTP date (like "Wed, 02 Jun 2010 06:19:37 GMT")
      u8 *date = (u8*)get_env(argv, SERVER_DATE);

      xbuf_xcat(reply,
                "HTTP/1.1 200 OK\r\n"
                "Date: %s\r\n"
                "Last-Modified: %s\r\n"
                "Content-type: image/gif\r\n"
                "Content-Length:        \r\n" // make room for the GIF length
                "Connection: close\r\n\r\n",
                date, date);
   }
   
   // -------------------------------------------------------------------------
   // make sure that we have enough space in the 'reply' buffer
   // (we are going to fill it directly from gif_build(), not via xbuf_xxx)
   // -------------------------------------------------------------------------
   // (if we have not enough memory, we will get a 'graceful' crash)
   if(reply->allocated < (img.w * img.h)) 
   {
      if(!xbuf_growto(reply, img.w * img.h)) // resize reply
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
   char *p = (reply->ptr + reply->len
         - (sizeof("\r\nConnection: close\r\n\r\n") - 1));

   // -------------------------------------------------------------------------
   // build and append a GIF image (-1:no transparency) to the 'reply' buffer
   // -------------------------------------------------------------------------
   int len = gif_build((u8*)reply->ptr + reply->len, img.bmp, img.w, img.h, 
                       (u8*)pal, 1 << img.bbp, -1, 0); // 0: no text comment
   if(len < 0) len = 0; // (len == -1) if gif_build() failed
   reply->len += len;   // add the GIF size to the 'reply' buffer length
   free(img.bmp);       // the raw bitmap is no longer needed
   
   // -------------------------------------------------------------------------
   // store the GIF size in the empty space of the 'Content-Length' header
   // -------------------------------------------------------------------------
   u32toa(p, len);

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
