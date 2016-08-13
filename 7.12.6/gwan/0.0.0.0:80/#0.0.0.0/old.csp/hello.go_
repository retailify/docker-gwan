// ============================================================================
// Go servlet example for the G-WAN Web Application Server (http://gwan.ch/)
// ----------------------------------------------------------------------------
// hello.go: G-WAN supports Google Go scripts
//
// In Google Go, main() can't have arguments nor it can return a value so we 
// use the os.Args array for the HTTP QUERY parameters and os.Getenv() to get
// the HTTP request and HTTP headers. The Go os.Exit() function lets servlets
// set a response HTTP code:
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
package main
 
import "fmt"
import "os"
 
// don't put the next brace down to the next line, that's a Go syntax error!
func main() {
	fmt.Printf("Hello, Go!<br><br>")

   // get environment variables
	// fmt.Printf("Hello %s (from Go)!<br><br>", os.Getenv("REMOTE_ADDR"))
	
	// list QUERY PARAMETERS, if any:
	//
	// GET http://10.10.200.80/?hello.go&arg1=123&arg2=456
	//
	// args[1] /opt/gwan/10.10.20.80_80/#192.168.200.80/csp/hello.go
	// args[2] arg1=123
	// args[3] arg2=456
	/*
	 for i := 1; i < len(os.Args); i++ {
       fmt.Printf("args[%d] %s<br>", i, os.Args[i])
    }
	*/
	
	os.Exit(200) // return an HTTP code (200:'OK')
}
// ============================================================================
// End of Source Code
// ============================================================================

