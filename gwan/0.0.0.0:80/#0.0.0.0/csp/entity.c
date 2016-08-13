// ============================================================================
// C servlet example for the G-WAN Web Application Server (http://www.gwan.ch/)
// ----------------------------------------------------------------------------
// entity.c: how to handle the POST/PUT/DELETE requests and their entity
//
//           We list the URI parameters and entiry (if any) and we decide what
//           to do with an entity loaded by G-WAN.
//
//           G-WAN will automatically load any entity up to a certain limit.
//           Entities largre than the limit are rejected with an HTTP error
//           413: "request Entity too large".
//
//           To load entities greater than 4KiB you will have to enlarge the
//           default maximum entity size (see entity_size.c for an example):
//
//              u32 *max_entity_size = (int*)get_env(argv, MAX_ENTITY_SIZE);
//              *max_entity_size = 200 * 1024; // in bytes (here 200 KiB)
//
//           Note that the URI parameters and the entity are processed in a
//           similar way by GET and PUT/POST/DELETE (both URI parameters and
//           an entity are reported in argc/argv[]). This is done purposely
//           to have a consistent behavior in all the cases.
// ============================================================================
// examples (using command 'GET' from package 'libwww-perl'):
//
// $ GET -m POST "127.0.0.1:8080/?entity.c"
// Please enter content (application/x-www-form-urlencoded) to be POSTed:
// 0123456[CRLF][Ctrl+D]
//
// <pre>
// HTTP method:      POST
// HTTP request:     POST /?put.c
// HTTP query:       put.c
// HTTP entity:      0123456[CRLF]
// Content-Encoding: 0: IDENTITY (default)
// Content-Length:   8
// Content-Type:     1: OCTETSTREAM
// by the client:    127.0.0.1
// Req. parameters:  1
//         argv[0]:  0123456
// </pre>
// ----------------------------------------------------------------------------
// $ GET -m PUT "127.0.0.1:8080/?entity.c&toto&titi"
// Please enter content (text/plain) to be PUTed:
// 0123456[CRLF][Ctrl+D]
//
// <pre>
// HTTP method:      PUT
// HTTP request:     PUT /?entity.c
// HTTP query:       entity.c
// HTTP entity:      0123456[CRLF]
// Content-Encoding: 0: IDENTITY (default)
// Content-Length:   8
// Content-Type:     0: plain text
// by the client:    127.0.0.1
// Req. parameters:  3
//         argv[0]:  toto
//         argv[1]:  titi
//         argv[2]:  0123456
// </pre>
// ----------------------------------------------------------------------------
// $ GET -m DELETE "127.0.0.1:8080/?entity.c&toto&titi"
//
// <pre>
// HTTP method:      DELETE
// HTTP request:     DELETE /?entity.c
// HTTP query:       entity.c
// HTTP entity:      (null)
// Content-Encoding: 0: IDENTITY (default)
// Content-Length:   0
// Content-Type:     0: plain text
// by the client:    127.0.0.1
// Req. parameters:  2
//         argv[0]:  toto
//         argv[1]:  titi
// </pre>
// ----------------------------------------------------------------------------
// example using wget:
//
// wget --post-file="127.0.0.1_8080/#127.0.0.1/www/100.html" "http://127.0.0.1:8080/?entity"
//
// POST /?entity HTTP/1.0
// User-Agent: Wget/1.12 (linux-gnu)
// Accept: */*
// Host: 127.0.0.1:8080
// Connection: Keep-Alive
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 100
//
// <pre>
// HTTP method:      POST
// HTTP request:     POST /?entity
// HTTP query:       entity
// HTTP entity:      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
// Content-Encoding: 0: IDENTITY (default)
// Content-Length:   100
// Content-Type:     1: OCTETSTREAM
// by the client:    127.0.0.1
// Req. parameters:  1
//         argv[0]:  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
// </pre><hr>
// <pre>
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// </pre><hr>
// ============================================================================
// examples using csp_entity.html
//
// HTTP method:      POST
// HTTP request:     POST /?entity&arg1=one&arg2=two
// HTTP query:       entity
// HTTP entity:     '-----------------------------2140458177142727869579064'...
// Content-Encoding: 0: IDENTITY (default)
// Content-Length:   2,662
// Content-Type:     2: MULTIPART
// by the client:    127.0.0.1
// Req. parameters:  3
//         argv[0]: '                                              arg1=one'
//         argv[1]: '                                              arg2=two'
//         argv[2]: '-----------------------------2140458177142727869579064'...
//
//      0: 2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------
//     16: 2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 32 31 34 | -------------214
//     32: 30 34 35 38 31 37 37 31  34 32 37 32 37 38 36 39 | 0458177142727869
//     48: 35 37 39 30 36 34 39 37  35 38 0D 0A 43 6F 6E 74 | 5790649758..Cont
//     64: 65 6E 74 2D 44 69 73 70  6F 73 69 74 69 6F 6E 3A | ent-Disposition:
//     80: 20 66 6F 72 6D 2D 64 61  74 61 3B 20 6E 61 6D 65 |  form-data; name
//     96: 3D 22 66 69 6C 65 2D 65  6E 74 69 74 79 22 3B 20 | ="file-entity";
//    112: 66 69 6C 65 6E 61 6D 65  3D 22 74 65 73 74 2E 62 | filename="test.b
//    128: 73 6F 6E 22 0D 0A 43 6F  6E 74 65 6E 74 2D 54 79 | son"..Content-Ty
//    144: 70 65 3A 20 61 70 70 6C  69 63 61 74 69 6F 6E 2F | pe: application/
//    160: 6F 63 74 65 74 2D 73 74  72 65 61 6D 0D 0A 0D 0A | octet-stream....
//    176: F6 08 00 00 05 67 75 69  64 00 10 00 00 00 05 28 | �....guid......(
//    192: 00 00 00 A8 1D E4 8F 01  00 00 00 00 95 78 06 05 | ...�.�.......x..
//    208: 63 6C 69 65 6E 74 2D 67  75 69 64 00 10 00 00 00 | client-guid.....
//    224: 05 B8 3C 68 AC 30 EB 4D  5E BA 86 A4 12 62 AA 9B | .�
//    ...
// ============================================================================
// HTTP method:      POST
// HTTP request:     POST /?entity&arg1=one&arg2=two
// HTTP query:       entity
// HTTP entity:     '-----------------------------8097748064733508259729864'...
// Content-Encoding: 0: IDENTITY (default)
// Content-Length:   548
// Content-Type:     2: MULTIPART
// by the client:    127.0.0.1
// Req. parameters:  3
//         argv[0]: '                                              arg1=one'
//         argv[1]: '                                              arg2=two'
//         argv[2]: '-----------------------------8097748064733508259729864'
//
// -----------------------------809774806473350825972986497
// Content-Disposition: form-data; name="file-entity"; filename="examples.desktop"
// Content-Type: application/octet-stream
//
// [Desktop Entry]
// Version=1.0
// Type=Link
// Name=Examples
// Comment=Example content for Ubuntu
// URL=file:///usr/share/example-content/
// Icon=folder
// X-Ubuntu-Gettext-Domain=example-content
//
//
// -----------------------------809774806473350825972986497
// Content-Disposition: form-data; name="submit"
//
// Send HTTP Request
// -----------------------------809774806473350825972986497--
//
//
// ============================================================================
#include "gwan.h"   // G-WAN exported functions
#include <stdio.h>     // puts()
#include <string.h>    // strcat()
#include <sys/ioctl.h> // ioctl()
#include <unistd.h>    // read()

#ifndef min
# define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

// see enum HTTP_Method {} and char *szHTTP_Method[] in gwan.h
static char *szHTTP_Method[] =
{
   [HTTP_ANY]        = "?",
   [HTTP_GET]        = "GET",
   [HTTP_HEAD]       = "HEAD",
   [HTTP_POST]       = "POST",
   [HTTP_PUT]        = "PUT",
   [HTTP_DELETE]     = "DELETE",
   [HTTP_OPTIONS]    = "OPTIONS",
   ""
};
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // we can get the same information with one unique call:
   //
   //   http_t *http = (http_t*)get_env(argv, HTTP_HEADERS);
   //
   // (see served_from.c for an example)
   //
   u32   method   = (u32)  get_env(argv, REQUEST_METHOD);
   char *cli_ip   = (char*)get_env(argv, REMOTE_ADDR);
   char *request  = (char*)get_env(argv, REQUEST);
   char *query    = (char*)get_env(argv, QUERY_STRING);
   char *entity   = (char*)get_env(argv, REQ_ENTITY);
   u32  cont_enc  = (u32)  get_env(argv, CONTENT_ENCODING);
   u32  cont_len  = (u32)  get_env(argv, CONTENT_LENGTH);
   u32  cont_type = (u32)  get_env(argv, CONTENT_TYPE);

   char szenc[40] = "IDENTITY (default)";
   if(cont_enc) *szenc = 0;
   if(cont_enc & ENC_GZIP)     strcat(szenc, "|GZIP");
   if(cont_enc & ENC_DEFLATE)  strcat(szenc, "|DEFLATE");
   if(cont_enc & ENC_COMPRESS) strcat(szenc, "|COMPRESS");
   if(cont_enc & ENC_CHUNKED)  strcat(szenc, "|CHUNKED");
   if(cont_enc & ENC_SDCH)     strcat(szenc, "|SDCH");

   char *sztype = "plain text"; // TYPE_IDENTIY (default)
   switch(cont_type)
   {
      case TYPE_URLENCODED: sztype = "URLENCODED";  break;
      case TYPE_MULTIPART:  sztype = "MULTIPART";   break;
      case TYPE_OCTETSTREAM:sztype = "OCTETSTREAM"; break;
   }

   xbuf_t *reply = get_reply(argv);
   xbuf_xcat(reply,
             "\n<pre>\n"
             "HTTP method:      %s\n"
             "HTTP request:     %s\n"
             "HTTP query:       %s\n"
             "HTTP entity:     '%.54s'%s\n"
             "Content-Encoding: %u: %s\n"
             "Content-Length:   %U\n"
             "Content-Type:     %u: %s\n"
             "by the client:    %s\n"
             "Req. parameters:  %d\n",
             szHTTP_Method[method],
             request, query,
             entity, cont_len > 54 ? "..." : "",
             cont_enc, szenc, cont_len, cont_type, sztype,
             cli_ip,
             argc);

   int i = 0;
   while(i < argc)
   {
      xbuf_xcat(reply, "        argv[%u]: '%.54s'\n", i, argv[i]);
      i++;
   }
   xbuf_cat(reply, "</pre><hr>\n\n");

   //puts(reply->ptr);

   // -------------------------------------------------------------------------
   // no entity, done!
   // -------------------------------------------------------------------------
   if(!entity)
      return HTTP_200_OK;

   // -------------------------------------------------------------------------
   // check if we have all the entity
   // -------------------------------------------------------------------------
   xbuf_t *read_buf = (xbuf_t*)get_env(argv, READ_XBUF);
   const u32 entity_offset = entity - read_buf->ptr;
   u32 curr_entity_len = read_buf->len - entity_offset;
   if(curr_entity_len < cont_len)
   {
      xbuf_xcat(reply, "<br>Entity[%k] is missing %k unread bytes\n",
                cont_len, curr_entity_len);
      return HTTP_200_OK;
   }

   // -------------------------------------------------------------------------
   // do something with the entity, like saving it on disk, etc.
   // -------------------------------------------------------------------------
   // here we just dump it in the HTML reply:

   i = cont_len > 512 ? 512 : cont_len;
   int to_dump = i; // limit the damage if people dump a 100MiB file...
   while(i--)
   {
      const u32 ch = entity[i];
      if(ch > 31 || ch == 13 || ch == 10 || ch == 9)
         continue; // exceptions for CR, LF, TAB

      // convert binary data into hexdump (printable) characters
      char *pos = reply->ptr + reply->len + 4;
      xbuf_xcat(reply, "<pre>\n%*v\n%s</pre>\n<hr>\n",
                to_dump, entity, cont_len > to_dump ? ".\n.\n." : "");

      // escape '<' because it cuts the text
      while(xbuf_replfrto(reply, pos, reply->ptr + reply->len - 13, "<", "&lt;"));

      return HTTP_200_OK; // return an HTTP code
   }

   // ASCII entity, send as is:
   char *pos = reply->ptr + reply->len + 4;
   xbuf_ncat(reply, "<pre>\n", sizeof("<pre>\n") - 1);
   xbuf_ncat(reply, entity, to_dump);
   xbuf_xcat(reply, "\n%s</pre><hr>\n", cont_len > to_dump ? ".\n.\n." : "");

   // escape '<' because it cuts the text
   while(xbuf_replfrto(reply, pos, reply->ptr + reply->len - 13, "<", "&lt;"));

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================

