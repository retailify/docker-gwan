// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// auth_basic.c: illustrating HTTP Basic authorization
//
// What is done in this example can also be done from G-WAN handlers, allowing
// you to dispatch, redirect, and block users depending on the requested URI,
// whether this is static (www) or dynamic (csp) contents.
//
// Notes:
//
// Basic authorization transmits credentials encoded in base64 so this cannot
// be considered to be safe if used with persistent passwords.
//
// Note that authentication is only one of the three components required for a
// secure transmission: (1) authentication, (2) confidentiality, (3) integrity.
//
// Not only you need to be sure of who you are talking to, but it also must be
// totally impossible for others to read or to alter exchanged messages (as 
// well as to replay old messages as if they were new).
//
// If one the premises above is not respected, then your communications are not
// secure. Any operation among those three key points that would be delegated 
// to remote third-parties must definitely be considered as unsafe because, by
// definition, what is done remotely by total strangers is out of your scope.
// ============================================================================
// usage:
//   
// Internet Browser: http://127.0.0.1:8080/?auth_basic
//   or
// curl -v --basic --user user:password http://127.0.0.1:8080/?auth_basic
//
// GET /?auth_basic HTTP/1.1
// Authorization: Basic dXNlcjpwYXNzd29yZA==
// Host: 192.168.200.80:8080
// Accept: */*
// 
// HTTP/1.1 200 OK
// Server: G-WAN
// Date: Sat, 23 Feb 2013 13:32:02 GMT
// Last-Modified: Sat, 23 Feb 2013 13:32:02 GMT
// ETag: "bf76cc4-5128c4d2-6f"
// Vary: Accept-Encoding
// Accept-Ranges: bytes
// Content-Type: text/html; charset=UTF-8
// Content-Length: 111
// [CRLF]
// <h1>Basic AUTHORIZATION</h1><p>Successfully authorized user:
// <tt>'user'</tt>, password: <tt>'password'</tt></p>
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN exported functions
#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------------------
// check if this user is authorized
// ----------------------------------------------------------------------------
// Instead of using hard-coded credentials, you can query a database,
// query an LDAP server, load a configuration file, etc.
static int valid_user(char *user, char *pwd, char *auth, char *uri, char *meth)
{
   // For higher security, passwords can be stored hashed and user rights
   // verified on a per URI basis:
   //
   // "acl": {
   // "roles:"
   //  [
   //   {"name":"admin", "description":"blah"},
   //   {"name":"guest", "description":"blah"},
   //   {"name":"manager", "description":"blah"},
   //   {"name":"moderator", "description":"blah"}
   //  ]
   //  "users:"
   //  [
   //   {"name":"Tom", "role":"admin" },
   //   {"name":"Tim", "role":"guest" }
   //  ]
   //  "rights:"
   //  [
   //   {"uri":"/?auth_basic", "auth":"Basic", "method":"GET", 
   //    "HA1":"012345678901234567890123456789AB"},
   //   {"uri":"/?auth_digest", "auth":"Digest", "method":"*",
   //    "HA1":"012345678901234567890123456789AB"}
   //  ]
   // }
   //
   // u8 HA1[32]; // md5_strs(HA1, "user:uri:password"); (stored in DB)
   // u8 HA2[32]; md5_strs(HA2, "user:uri:password"); // live from client
   // char role[64] = {0};
   // return !(!user || !pwd
   //     || !user_find(user, role)                 // is 'user' registered
   //     || !user_acls(user, uri, auth, meth, HA1) // can 'user' reach 'uri'
   //     || memcmp(HA1, HA2, 32));                 // hashed passwords test
   //
   return !(!user || !pwd
        || memcmp(meth, "GET", 3)    // hard-coded values make this 
        || memcmp(auth, "Basic", 5)  // example easier to understand
        || strcmp(user, "user")      // unknown length makes memcmp() unsafe
        || strcmp(pwd, "password")); // unknown length makes memcmp() unsafe
}
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   xbuf_t *reply = get_reply(argv);
   http_t *http = (void*)get_env(argv, HTTP_HEADERS);
   
   // -------------------------------------------------------------------------
   // here you could count repeated attempts from the same IP address and then
   // take measures like blacklisting offenders (using G-WAN's TCP/IP firewall)
   // -------------------------------------------------------------------------
   // u32 client_ip = (u32)get_env(argv, REMOTE_BIN_ADDR); // binary
   // char *client_ip = (char*)get_env(argv, REMOTE_ADDR); // string
   
   // -------------------------------------------------------------------------
   // first (unauthorized) client request or something went bad
   // -------------------------------------------------------------------------
   // /?auth_digest might be called by rogue clients, without G-WAN having
   // received (and validated) an authorization request
   if(http->h_auth_type != AUTH_BASIC)
      goto ask_new_auth;

   // -------------------------------------------------------------------------
   // find the request parameters we need (auth_type is double-checked here
   // but that's to show how to implement a generic valid_user() function
   // aimed at being used with a database or an LDAP server)
   // -------------------------------------------------------------------------
   static char *auth_types[] =
   {
      [AUTH_BAD]    = "",
      [AUTH_ANY]    = "*",
      [AUTH_BASIC]  = "Basic",
      [AUTH_DIGEST] = "Digest",
      [AUTH_SRP]    = "SRP",
      [AUTH_X509]   = "x509"
   };
   if(http->h_auth_type < AUTH_BASIC // check before you lookup auth_types[]
   || http->h_auth_type > AUTH_X509)
      http->h_auth_type = AUTH_BAD;

   char *auth = auth_types[http->h_auth_type];
   char *meth = (char*)get_env(argv, REQUEST); // "GET /?auth_digest"
   char *uri  = strchr(meth, ' ');             //     ^
   if(uri) uri++;                              // pass space character
   
   // -------------------------------------------------------------------------
   // check if the user is authorized (if not, send 401)
   // -------------------------------------------------------------------------
   if(!valid_user(http->h_auth_user, http->h_auth_pwd, auth, uri, meth))
      goto ask_new_auth;

   // -------------------------------------------------------------------------
   // user is authorized, produce contents, redirect to contents, etc.
   // -------------------------------------------------------------------------
   xbuf_xcat(reply, "<h1>Basic AUTHORIZATION</h1><p>Successfully authorized" 
                   " user: <tt>'%s'</tt>, password: <tt>'%s'</tt></p>",
                   http->h_auth_user, http->h_auth_pwd);

   return HTTP_200_OK; // return an HTTP code
   
   // -------------------------------------------------------------------------
   // first (unauthorized) client request or something went bad
   // -------------------------------------------------------------------------
ask_new_auth:

   // let G-WAN generate a 401 server reply with the authorization we want
   http->h_auth_type = AUTH_BASIC;
   return HTTP_401_UNAUTHORIZED;
}
// ============================================================================
// End of Source Code
// ============================================================================

