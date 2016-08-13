// ============================================================================
// Go servlet example for the G-WAN Web Application Server (http://gwan.ch/)
// ----------------------------------------------------------------------------
// hello.js: G-WAN supports Javascript scripts
//
// ----------------------------------------------------------------------------
// CGI/1.1 environment variables:
// ----------------------------------------------------------------------------
// "AUTH_TYPE",          // "" | "Basic" | "Digest" | etc.
// "CONTENT_LENGTH",     // "" | entity_length
// "CONTENT_TYPE",       // "" | content_type
// "GATEWAY_INTERFACE",  // "CGI/1.1"
// "PATH_INFO",          // "" | ( "/" path )
// "PATH_TRANSLATED",    // disk filename for PATH_INFO
// "QUERY_STRING",       // "" | ?"hellox.c&name=toto"
// "REMOTE_ADDR",        // client IP address
// "REMOTE_HOST",        // client DNS name (or IP addr)
// "REMOTE_IDENT",       // client identity (RFC 1413), opt
// "REMOTE_USER",        // client identity (if auth)
// "REQUEST_METHOD",     // "GET" | "HEAD" | "PUT", etc.
// "SCRIPT_NAME",        // "" | ("/" path "hello.c")
// "SERVER_NAME",        // "gwan.com" | IP address
// "SERVER_PORT",        // "80"
// "SERVER_PROTOCOL",    // "HTTP/1.1" | "HTTP/1.0" | "HTTP/0.9"
// "SERVER_SOFTWARE",    // "G-WAN"
// ----------------------------------------------------------------------------

   console.log("Hello, Javascript!<br>")

   // get environment variables
   //console.log("<br>USER: %s<br>", process.env.USER);
	//console.log("REMOTE_ADDR: %s<br>", process.env.REMOTE_ADDR);
	
	// list QUERY PARAMETERS, if any:
	//
	// GET http://10.10.200.80/?hello.go&arg1=123&arg2=456
	//
   // args[0] node
	// args[1] /opt/gwan/10.10.20.80_80/#192.168.200.80/csp/hello.js
	// args[2] arg1=123
	// args[3] arg2=456
   /*	
   console.log("<br>")
	process.argv.forEach(function(val, index, array) {
       console.log("args[" + index + "]: " + val + "<br>")
   });
   */

   process.exit(200);

// ============================================================================
// End of Source Code
// ============================================================================

