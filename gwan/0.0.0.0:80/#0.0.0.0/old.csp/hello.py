# ============================================================================
# Python servlet example for the G-WAN Web Application Server: http://gwan.ch/
# ----------------------------------------------------------------------------
# hello.py: G-WAN supports Python scripts
#
# Don't indent lines outside of a loop/test, that's a syntax error in Python.
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

import os
import sys

print 'Hello, Python!<br>'

# get environment variables
# 
# print '<pre>SCRIPT_NAME: ', os.environ.get('SCRIPT_NAME'), '</pre><br>'

# list QUERY PARAMETERS, if any:
#
# GET http://10.10.200.80/?hello.pl&arg1=123&arg2=456
#
# arg[0]: /.../csp/hello/py
# arg[1]: arg1=123
# arg[2]: arg2=456
#
# print "<pre>argc:", len(sys.argv), "<br>"
# for arg in sys.argv :
#    print "argv[]: ", arg, "<br>"
# print "</pre>"

sys.exit(200)

# ============================================================================
# End of Source Code
# ============================================================================

