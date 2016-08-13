# ============================================================================
# Ruby servlet example for the G-WAN Web Application Server (http://gwan.ch/)
# ----------------------------------------------------------------------------
# hello.rb: G-WAN supports Ruby scripts
# 
# This string concatenation method used below is less convenient than a '+' 
# but it avoids a NoMethod error if the variable is 'nil'.
# 
# CGI/1.1 environment variables:
# ----------------------------------------------------------------------------
# "AUTH_TYPE",          // "" | "Basic" | "Digest" | etc.
# "CONTENT_LENGTH",     // "" | entity_length
# "CONTENT_TYPE",       // "" | content_type
# "GATEWAY_INTERFACE",  // "CGI/1.1"
# "PATH_INFO",          // "" | ( "/", "/dir", etc. URI path)
# "PATH_TRANSLATED",    // disk filename for PATH_INFO
# "QUERY_STRING",       // "" | ?"hellox.c&name=toto"
# "REMOTE_ADDR",        // client IP address
# "REMOTE_HOST",        // client DNS name (or IP addr)
# "REMOTE_IDENT",       // client identity (RFC 1413), opt
# "REMOTE_USER",        // client identity (if auth)
# "REQUEST_METHOD",     // "GET" | "HEAD" | "PUT", etc.
# "REQUEST_URI",        // "/index.html"
# "SCRIPT_NAME",        // "" | ("/" path "hello.c")
# "SERVER_NAME",        // "gwan.com" | IP address
# "SERVER_PORT",        // "80"
# "SERVER_PROTOCOL",    // "HTTP/1.1" | "HTTP/1.0" | "HTTP/0.9"
# "SERVER_SOFTWARE",    // "G-WAN"
# "X_HTTP_HEADERS",     // HTTP request Headers (G-WAN extension)
# ----------------------------------------------------------------------------

   puts 'Hello, Ruby!<br><br>'

   # (here we get the full PATH of the script using a Ruby variable)
   # puts "$0: #{$0}<br><br>"

   # get environment variables
   puts "<pre>" 
   puts "AUTH_TYPE: #{ENV['AUTH_TYPE']}<br>"
   puts "CONTENT_LENGTH: #{ENV['CONTENT_LENGTH']}<br>"
   puts "CONTENT_TYPE: #{ENV['CONTENT_TYPE']}<br>"
   puts "GATEWAY_INTERFACE: #{ENV['GATEWAY_INTERFACE']}<br>"
   puts "PATH_INFO: #{ENV['PATH_INFO']}<br>"
   puts "PATH_TRANSLATED: #{ENV['PATH_TRANSLATED']}<br>"
   puts "QUERY_STRING: #{ENV['QUERY_STRING']}<br>"
   puts "REMOTE_ADDR: #{ENV['REMOTE_ADDR']}<br>"
   puts "REMOTE_HOST: #{ENV['REMOTE_HOST']}<br>"
   puts "REMOTE_IDENT: #{ENV['REMOTE_IDENT']}<br>"
   puts "REMOTE_USER: #{ENV['REMOTE_USER']}<br>"
   puts "REQUEST_METHOD: #{ENV['REQUEST_METHOD']}<br>"
   puts "REQUEST_URI: #{ENV['REQUEST_URI']}<br>"
   puts "SCRIPT_NAME: #{ENV['SCRIPT_NAME']}<br>"
   puts "SERVER_NAME: #{ENV['SERVER_NAME']}<br>"
   puts "SERVER_PORT: #{ENV['SERVER_PORT']}<br>"
   puts "SERVER_PROTOCOL: #{ENV['SERVER_PROTOCOL']}<br>"
   puts "SERVER_SOFTWARE: #{ENV['SERVER_SOFTWARE']}<br>"
   puts "REMOTE_ADDR: #{ENV['REMOTE_ADDR']}<br>"
   puts "<hr>"
   puts "X_HTTP_HEADERS:<br><br>#{ENV['X_HTTP_HEADERS']}<br>"
   puts "</pre><hr><br>"

   # list QUERY PARAMETERS, if any:
   #
   # GET http://10.10.200.80/?hello.rb&arg1=123&arg2=456
   #
   # arg: arg1=123
   # arg: arg2=456
   #
   # ARGV.each do|a|
   #   puts "arg: #{a}<br>"
   # end
   
   exit(200); # return an HTTP code (200:'OK')

# ============================================================================
# End of Source Code
# ============================================================================

