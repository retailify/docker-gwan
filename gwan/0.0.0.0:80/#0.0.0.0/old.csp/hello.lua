-- ============================================================================
-- Go servlet example for the G-WAN Web Application Server (http://gwan.ch/)
-- ----------------------------------------------------------------------------
-- hello.go: G-WAN supports Lua scripts
--
-- The use of tostring() is madatory to avoid a crash when a variable is NULL,
-- a condition that Lua calls 'nil'.
--
-- ----------------------------------------------------------------------------
-- CGI/1.1 environment variables:
-- ----------------------------------------------------------------------------
-- "AUTH_TYPE",          // "" | "Basic" | "Digest" | etc.
-- "CONTENT_LENGTH",     // "" | entity_length
-- "CONTENT_TYPE",       // "" | content_type
-- "GATEWAY_INTERFACE",  // "CGI/1.1"
-- "PATH_INFO",          // "" | ( "/" path )
-- "PATH_TRANSLATED",    // disk filename for PATH_INFO
-- "QUERY_STRING",       // "" | ?"hellox.c&name=toto"
-- "REMOTE_ADDR",        // client IP address
-- "REMOTE_HOST",        // client DNS name (or IP addr)
-- "REMOTE_IDENT",       // client identity (RFC 1413), opt
-- "REMOTE_USER",        // client identity (if auth)
-- "REQUEST_METHOD",     // "GET" | "HEAD" | "PUT", etc.
-- "SCRIPT_NAME",        // "" | ("/" path "hello.c")
-- "SERVER_NAME",        // "gwan.com" | IP address
-- "SERVER_PORT",        // "80"
-- "SERVER_PROTOCOL",    // "HTTP/1.1" | "HTTP/1.0" | "HTTP/0.9"
-- "SERVER_SOFTWARE",    // "G-WAN"
-- ----------------------------------------------------------------------------

   io.stdout:write("Hello, Lua!<br>")

-- get environment variables
-- io.stdout:write("user:", os.getenv("USER"), "<br>")
-- io.stdout:write("server:", tostring(os.getenv("SERVER_SOFTWARE")), "<br>")
-- io.stdout:write("query:", tostring(os.getenv("QUERY_STRING")), "<br><br>")

-- list QUERY PARAMETERS, if any:
--
-- GET http://10.10.200.80/?hello.lua&arg1=123&arg2=456
--
-- args[1] arg1=123
-- args[2] arg2=456
--
-- for k,v in ipairs (arg) do
--    io.stdout:write("arg[",tostring(k),"]: ",tostring(v),"<br>")
-- end

   os.exit(200) -- return an HTTP code (200:'OK')

-- ============================================================================
-- End of Source Code
-- ============================================================================

