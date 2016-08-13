// ============================================================================
// This is a Servlet sample for the G-WAN Web Server (http://www.trustleap.com)
// ----------------------------------------------------------------------------
// contact.c: Build dynamic HTML pages to process a 'Contact Form'
//
//            GET and POST forms are processed with the SAME code (the server
//            does the URL/Entity parsing for Request Handlers like contact.c).
//
//            When the form is sent to the server, we use the form fields
//            (and the client IP address) to build an email which is sent
//            to your SMTP server. A feedback page is then sent to clients.
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//   xbuf_init(): called after xbuf_t has been declared, to initialize struct
// xbuf_frfile(): load a file and append its contents to a specified buffer
//   xbuf_repl(): replace a string by another string in a specified buffer
//    xbuf_cat(): like strcat(), but in the specified dynamic buffer 
//   xbuf_ncat(): like strncat(), but in the specified dynamic buffer 
//   xbuf_xcat(): formatted strcat() (a la printf) in a given dynamic buffer 
//   xbuf_free(): release the memory allocated for a dynamic buffer
//     get_arg(): get the specified form field value
//    sendmail(): send mail for relaying to the 'from' mail address' SMTPserver
//   s_asctime(): like asctime(), but thread-safe
//     get_env(): get connection's 'environment' variables from the server
// ----------------------------------------------------------------------------
#include "gwan.h" // G-WAN exported functions

#include <malloc.h>
#include <string.h>

// ----------------------------------------------------------------------------
static void remove_crlfs(char *s)
{
   char *d = s;
   for(; *s; s++)
      if(*s != '\r' && *s != '\n') *d++ = *s;
   *d = 0;
}
// ----------------------------------------------------------------------------
// main()
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // get a pointer on the server response buffer
   xbuf_t *reply = get_reply(argv);

   xbuf_t f;      // create a dynamic buffer
   xbuf_init(&f); // initialize buffer

// int   client_port = (int)  get_env(argv, REMOTE_PORT);
   char *client_ip   = (char*)get_env(argv, REMOTE_ADDR);

   // -------------------------------------------------------------------------
   // no URL parameters, we have to send the initial "Contact Form"
   // -------------------------------------------------------------------------
   if(argc < 2)
   {
      // a template HTML file, with fields that will be replaced by variables 
      char *file = "/csp_contact.html", str[1024], tmp[80];

      // open the template HTML file located under ".../www/csp_contact.html"
      char *wwwpath = (char*)get_env(argv, WWW_ROOT); // get ".../www/" path
      s_snprintf(str, 1023, "%s/%s", wwwpath, file);  // build full file path
      xbuf_frfile(&f, str);                           // load file in buffer
      if(f.len)                                       // load succeeded?
      {
         // build the time and IP address strings
         sprintf(str, "Our current time is: %s", s_asctime(0, tmp));
         sprintf(tmp, "Your IP address is: %s",  client_ip);
         
         xbuf_repl(&f, "<!--time-->", str);  // replace field1 by variable
         xbuf_repl(&f, "<!--ip-->",   tmp);  // replace field2 by variable
         xbuf_ncat(reply, f.ptr, f.len);     // dump file into HTML page
         xbuf_free(&f);                      // free dynamic buffer

         return HTTP_200_OK; // return an HTTP code
      }
      
      return HTTP_404_NOT_FOUND; // return an HTTP code
   }

   // -------------------------------------------------------------------------
   // if we have URL parameters, we must process a 'POST' Form
   // -------------------------------------------------------------------------

   // the form field "names" we want to find values for 
   char *url="", *email="", *subject="", *text="";
   get_arg("url=",     &url,     argc, argv); // note the ending '='
   get_arg("email=",   &email,   argc, argv);
   get_arg("subject=", &subject, argc, argv);
   get_arg("text=",    &text,    argc, argv);

   // SPAMmers use CRLFs in Web form fields to inject more destinees
   remove_crlfs(url);
   remove_crlfs(email);
   remove_crlfs(subject);
   remove_crlfs(text);
  
   // Build the Title and the Top of our HTML page
   static char title []="Contact Form";
   static char top[]="<!DOCTYPE HTML>"
       "<html lang=\"en\"><head><title>%s</title><meta http-equiv"
       "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
       "<link href=\"/imgs/style.css\" rel=\"stylesheet\" type=\"text/css\">"
       "</head><body style=\"margin:16px;\"><h2>%s</h2>";
   xbuf_xcat(reply, top, title, title);

   // insert useful information like which language was used (the 'url' arg)
   // and which IP address the client has used to send this email
   xbuf_xcat(&f, "From:%s in '%s'\n---\n%s", client_ip, url, text);

   xbuf_ctx subj;     // create a dynamic buffer
   xbuf_reset(&subj); // initialize buffer
   xbuf_xcat(&subj, "Contact - %s", subject);

   // send the form data to your mail server (nobody will spam you if
   // this address is known only by this program and your mail server)
   char *error = (char*)malloc(2048 + f.len);
   if(!error)
   {
      xbuf_free(&f);
      xbuf_free(&subj);
      log_err(argv, "webmail: out of memory");
      return HTTP_500_INTERNAL_SERVER_ERROR;
   }
   
/*
   if(sendemail("smtp.server.com",       // smtp server
                email,                   // src email address
                "contact@server.com",    // dst email address
                subj.ptr, f.ptr,         // email title / text
                "contact@trustleap.com", // username 
                "secret_password",       // password
                error))
   {
      xbuf_free(&f);
      xbuf_free(&subj);
      xbuf_cat(&reply, "<p>Your Email could not be sent.<br>"
                       "Please send a fax for important matters."
                       "</p></fieldset></body></html>");
      char str[1024];
      s_snprintf(str, sizeof(str)-1, "contact:error: '%s'", error);
      log_err(argv, str);
      free(error);
      return HTTP_200_OK;
   }*/
   free(error);
   xbuf_free(&f);
   xbuf_free(&subj);

   // send feedback to your correspondant and close the HTML page
   xbuf_xcat(reply, 
            "<br><fieldset style=\"width:640px;\">"
            "<p>Thank you for contacting us!</p>"
            "<p>You have sent:</p><hr>"
            "<pre>EMAIL: %H</pre><hr>"   // "%H" escapes HTML
            "<pre>SUBJECT: %H</pre><hr>"
            "<pre>TEXT: %H</pre>"
            "</fieldset></body></html>", email, subject, text);

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
