<?php 
// ============================================================================
// PHP servlet example for the G-WAN Web Application Server (http://gwan.ch/)
// ----------------------------------------------------------------------------
// hello.PHP: G-WAN supports PHP scripts
//
// If you need to get URI parameters you can either use if(PHP_SAPI==='cli')
// as shown below to parse and store them in an PHP array, or you can just
// use argc and argv as done below in the code.
//
// echo takes '' and "" (but needs the . concatenator between static strings
// and variables). fwrite() only accepts "" but works without separators so
// we stick to fwrite() which has a simpler syntax and which is likely to
// perform better (echo is most probably a wrapper).
//
// $_ENV does not define the CGI variables setup by G-WAN like 'QUERY_STRING',
// but I have found $_SERVER which does the work.
// ============================================================================
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

   // convert PHP command-line arguments into PHP URI parameters
   // (replace $_GET by $_POST if you are processing a POST REQUEST)
   // see loan.php for an example
   /*
   if(PHP_SAPI==='cli') :
      parse_str(implode('&', array_slice($argv, 1)), $_GET);
   endif;
   */

   fwrite(STDOUT, "Hello, PHP!<br>");
   
   // get environment variables (URI parameters)
	/*
	$name = isset($_GET['name']) ? $_GET['name'] : "?";
	fwrite(STDOUT, "<br>Hello $name <br><br>");
	*/
	
   // get environment variables (CGI environment)
   // ($_ENV['SERVER_SOFTWARE'] is undefined...)
	/*
	$server = isset($_SERVER['SERVER_SOFTWARE']) ? $_SERVER['SERVER_SOFTWARE'] : "?";
	fwrite(STDOUT, "Using server: $server <br><br>");
	*/

	// list QUERY PARAMETERS, if any:
	//
	// GET http://10.10.200.80/?hello.php&arg1=123&arg2=456
	//
	// args[2] arg1=123
	// args[3] arg2=456
	/*
	fwrite(STDOUT, "Got $argc URI Parameters:<br><pre>");
   foreach ($argv as $k => $v) {
      fwrite(STDOUT, "arg[$k]: $v<br>");
   }   
	fwrite(STDOUT, "</pre>");
	*/

   exit(200); // return an HTTP code (200:'OK')

// ============================================================================
// End of Source Code
// ============================================================================
?>


