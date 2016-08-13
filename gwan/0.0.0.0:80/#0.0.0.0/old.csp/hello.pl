# ============================================================================
# Perl servlet example for the G-WAN Web Application Server (http://gwan.ch/)
# ----------------------------------------------------------------------------
# hello.pl: G-WAN supports Perl scripts
#
# CGI/1.1 environment variables:
# ----------------------------------------------------------------------------
# "AUTH_TYPE",          // "" | "Basic" | "Digest" | etc.
# "CONTENT_LENGTH",     // "" | entity_length
# "CONTENT_TYPE",       // "" | content_type
# "GATEWAY_INTERFACE",  // "CGI/1.1"
# "PATH_INFO",          // "" | ( "/" path )
# "PATH_TRANSLATED",    // disk filename for PATH_INFO
# "QUERY_STRING",       // "" | ?"hellox.c&name=toto"
# "REMOTE_ADDR",        // client IP address
# "REMOTE_HOST",        // client DNS name (or IP addr)
# "REMOTE_IDENT",       // client identity (RFC 1413), opt
# "REMOTE_USER",        // client identity (if auth)
# "REQUEST_METHOD",     // "GET" | "HEAD" | "PUT", etc.
# "SCRIPT_NAME",        // "" | ("/" path "hello.c")
# "SERVER_NAME",        // "gwan.com" | IP address
# "SERVER_PORT",        // "80"
# "SERVER_PROTOCOL",    // "HTTP/1.1" | "HTTP/1.0" | "HTTP/0.9"
# "SERVER_SOFTWARE",    // "G-WAN"
# ----------------------------------------------------------------------------

   print "Hello, Perl!<br>";

   # get environment variables
   # 
   # print "SCRIPT_NAME: $ENV{SCRIPT_NAME}<br>";
   # print "<br><pre>";
   # foreach $key (keys %ENV) {
   #    print "$key: $ENV{$key}<br>";
   # }
   # print "</pre>";

   # list QUERY PARAMETERS, if any:
   #
   # GET http://10.10.200.80/?hello.pl&arg1=123&arg2=456
   #
   # arg[0]: arg1=123
   # arg[1]: arg2=456
   #
   # $nbrArgs = $#ARGV + 1;
   # print "<br><pre>argc:$nbrArgs<br>";
   # foreach $argnum (0 ... $#ARGV) {
   #    print "argv[$argnum]: $ARGV[$argnum]<br>";
   # }
   # print "</pre>";
   
   exit 200; # return an HTTP code (200:'OK')

# ============================================================================
# End of Source Code
# ============================================================================

