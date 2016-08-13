
This directory contains the following types of G-WAN handlers:

 1. main.c ... connection handler ...... alter the default behavior of G-WAN
 2. html.c ... content-type handlers ... called before a MIME type is served
 3. PONG.c ... protocol handlers ....... implementing a protocol: LDAP, POP3...

   Connection handlers are unique per listener and let you do URI rewrites, IP
   address filters, etc. when using G-WAN's default protocol: HTTP.

   Content-Type handlers can be as numerous as MIME types, and can be used to
   do processing like encryption, compression, etc.

   Protocol handlers define how connections will be handled by G-WAN. A single
   connection can dynamically change its protocol during its lifetime.
   
To disable a handler, just rename its file extension to something like *.c_ or
*.cpp_ (with one or several underscores).

For more information, read the http://gwan.ch/faq or the PDF manual available
at http://gwan.ch/download

