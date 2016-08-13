// ============================================================================
// This is a Servlet sample for the G-WAN Web Server (http://www.trustleap.com)
// ----------------------------------------------------------------------------
// email.c: How to send an email
//
// When your ISP is blocking port 25, use "smtp.trustleap.com:587" to define
// another port number to use.
// ============================================================================
#include "gwan.h" // G-WAN exported functions

#include <malloc.h>
#include <stdio.h>
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   char *dst_address[] = { "john@doe.com", "", 0 }, 
         subject[] = "test", text[] = "Test.";
   xbuf_t *reply = get_reply(argv);

   // Check the email address syntax
   if(!isvalidemailaddr(*dst_address))
   {                    
      xbuf_cat(reply, "<p>Invalid Email address.<br>"
                       "Please try again.</p>");
      return HTTP_200_OK; // return an HTTP code
   }

   char *error = (char*)malloc(2048 + sizeof(text));
   if(!error)
      return HTTP_500_INTERNAL_SERVER_ERROR;
      
   // -------------------------------------------------------------------
   // if you have your own SMTP server on the machine hosting G-WAN:
   // -------------------------------------------------------------------
   // apt-get install mailutils     (to have 'mailx')
   // echo "mail body" | mailx -s "subject" "me@dom.com"
   //
   // # attach '/tmp/logs.bz2' and and mail it
   // uuencode /tmp/logs.bz2 /tmp/logs.bz2 | mailx -s "subject" "me@dom.com"
   // 
   // # mail 'img.png' with the text message 'body.txt'
   // (cat body.txt; uuencode img.png img.png) | mailx -s "subject" "me@dom.com"

   // -------------------------------------------------------------------
   // if you DO NOT have an SMTP server on the machine hosting G-WAN:
   // -------------------------------------------------------------------
// if(sendemail("smtp.domain.com:587", // smtp server with custom port
   if(sendemail("smtp.domain.com",     // smtp server
                "pierre@domain.com",   // src. email address
                dst_address,           // dst. email address
                subject, text,         // email title / text
                sizeof(text) - 1,      // text length
                0,                     // attachments
                "pierre@domain.com",   // username for login
                "secret",              // password for login
                CAP_AUTH_PLAIN,        // Auth. method
                error))                // error storage space
   {
      printf("sendemail() error:%s\n", error);
      xbuf_xcat(reply, "<p>Your Email could not be sent: %s.</p>", error);
      free(error);
      return HTTP_200_OK;
   }

   free(error);
   xbuf_cat(reply, "<p>Email sent.</p>");
   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================

