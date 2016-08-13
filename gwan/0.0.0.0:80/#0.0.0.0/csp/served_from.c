// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// servedfrom.c: return "Served from 192.168.142.16:8080" (with your details)
//
// This script makes use of many get_env() values and compares this method to
// the http_t structure to fetch HTTP request headers.
// ============================================================================
// imported functions:
//     get_env(): get the specified 'environment' variable from the server
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//   xbuf_xcat(): like sprintf(), but it works in the specified dynamic buffer 
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN exported functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// ----------------------------------------------------------------------------
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
   // G-WAN currently supports this list until 'OPTIONS'
   [HTTP_CONNECT]    = "CONNECT",
   [HTTP_TRACE]      = "TRACE",
   [HTTP_PATCH]      = "PATCH",     // no RFC (remove?)
   [HTTP_PROPFIND]   = "PROPFIND",  // RFC 2518: WebDAV
   [HTTP_PROPPATCH]  = "PROPPATCH",
   [HTTP_MKCOL]      = "MKCOL",
   [HTTP_COPY]       = "COPY",
   [HTTP_MOVE]       = "MOVE",
   [HTTP_LOCK]       = "LOCK",
   [HTTP_UNLOCK]     = "UNLOCK",
   [HTTP_VERSION_CONTROL]="VERSION_CONTROL",
   [HTTP_CHECKOUT]   = "CHECKOUT",
   [HTTP_UNCHECKOUT] = "UNCHECKOUT",
   [HTTP_CHECKIN]    = "CHECKIN",
   [HTTP_UPDATE]     = "UPDATE",
   [HTTP_LABEL]      = "LABEL",
   [HTTP_REPORT]     = "REPORT",
   [HTTP_MKWORKSPACE]= "MKWORKSPACE",
   [HTTP_MKACTIVITY] = "MKACTIVITY",
   [HTTP_BASELINE_CONTROL]="BASELINE_CONTROL",
   [HTTP_MERGE]      = "MERGE",
   [HTTP_INVALID]    = "INVALID"    // RFC 3253: WebDAV versioning
   ""
};
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   xbuf_t *reply = get_reply(argv);

   char *srv_host = (char*)get_env(argv, SERVER_NAME);
   char *srv_ip   = (char*)get_env(argv, SERVER_ADDR);
   u32 srv_port   = (u32)  get_env(argv, SERVER_PORT);
   char *vhost    = (char*)get_env(argv, VHOST_ROOT);
   char *cli_ip   = (char*)get_env(argv, REMOTE_ADDR);
   u32 cli_port   = (u32)  get_env(argv, REMOTE_PORT); 
   u32   method   = (u32)  get_env(argv, REQUEST_METHOD);
   char *cli_id   = (char*)get_env(argv, USER_AGENT);
   char *request  = (char*)get_env(argv, REQUEST);
   char *query    = (char*)get_env(argv, QUERY_STRING);
   char *entity   = (char*)get_env(argv, REQ_ENTITY);
   u32  cont_enc  = (u32)  get_env(argv, CONTENT_ENCODING);
   u32  cont_len  = (u32)  get_env(argv, CONTENT_LENGTH);
   u32  cont_type = (u32)  get_env(argv, CONTENT_TYPE);

   xbuf_xcat(reply,
             "\n<pre>\n"
             "This page was processed...\n\n"
             "<b>Using get_env()</b>:\n\n"
             "by the Server:    %s:%u (hostname: %s)\n"
             "Virtual Host:     %s\n"
             "HTTP method:      %s\n"
             "HTTP request:     %s\n"
             "HTTP query:       %s\n"
             "HTTP entity:      %s\n"
             "Content-Encoding: %u\n"
             "Content-Length:   %U\n"
             "Content-Type:     %u\n"
             "for the Client:   %s:%u\n"
             "%s\n\n",
             srv_ip, srv_port, srv_host,
             vhost,
             szHTTP_Method[method],
             request, 
             query,
             entity,
             cont_enc,
             cont_len, cont_type,
             cli_ip, cli_port, cli_id);

   // -------------------------------------------------------------------------
   // an alternate, faster method to get the similar information:
   // -------------------------------------------------------------------------
   // (see typedef struct http_t in gwan.h)
   //
   http_t *http = (http_t*)get_env(argv, HTTP_HEADERS);
   
   // string HTTP headers are just offsets in the request
   const char *req_ptr = request - strlen(szHTTP_Method[method]);
   
   // get string pointers in the request for some HTTP headers
   const char *h_host = http->h_host ? req_ptr + http->h_host : "-";
   const char *h_entity = http->h_entity ? req_ptr + http->h_entity : "-";
   const char *h_agent = http->h_useragent ? req_ptr + http->h_useragent : "-";
   const char *h_cookies = http->h_cookies ? req_ptr + http->h_cookies : "-";
   const char *h_accept_language = http->h_accept_language ?
                                   req_ptr + http->h_accept_language : "-";

   // this is how you can interpret numeric values for encodings   
   char acpt_encodings[80] = {0};
   if(http->h_accept_encoding & ENC_GZIP)     strcat(acpt_encodings, "|GZIP");
   if(http->h_accept_encoding & ENC_DEFLATE)  strcat(acpt_encodings, "|DEFLATE");
   if(http->h_accept_encoding & ENC_COMPRESS) strcat(acpt_encodings, "|COMPRESS");
   if(http->h_accept_encoding & ENC_CHUNKED)  strcat(acpt_encodings, "|CHUNKED");
   if(http->h_accept_encoding & ENC_SDCH)     strcat(acpt_encodings, "|SDCH");

   xbuf_xcat(reply,
             "\n" 
             "<b>Using HTTP Headers to get the same information</b>:\n\n"
             "by the Server:    %s:%u (hostname: %s)\n"
             "HTTP method:      %s\n"
             "HTTP entity:      %s\n"
             "Content-Encoding: %u\n"
             "Content-Length:   %U\n"
             "Content-Type:     %u\n"
             "for the Client:   %s:%u\n"
             "%s\n"
             "Acpt-Language:    %s\n"
             "Acpt-Encoding:    %u: %s\n"
             "Cookies:          %s\n</pre>\n\n",
             h_host, (u32)http->h_port, h_host, 
             szHTTP_Method[http->h_method],
             h_entity, 
             http->h_content_encoding,
             http->h_content_length,
             http->h_content_type,
             cli_ip, cli_port, 
             h_agent,
             h_accept_language,
             http->h_accept_encoding, acpt_encodings,
             h_cookies);

   return 200;
}
// ============================================================================
// End of Source Code
// ============================================================================
