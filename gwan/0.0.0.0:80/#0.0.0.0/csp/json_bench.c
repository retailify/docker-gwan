// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// json_bench.c: just parsing json
//
// output:
// On 1 single CPU Core, parsing this JSON tree 100,000 times took 624 ms
// ============================================================================
#include "gwan.h" // G-WAN definitions

static char s_json[] = { 
   "{"
      "\"bid_request\":{"
         "\"members\":["
            "{"
               "\"id\":5"
            "}"
         "],"
         "\"allow_exclusive\":false,"
         "\"debug_requested\":false,"
         "\"tags\":["
            "{"
               "\"id\":13,"
               "\"sizes\":[\"728x90\"],"
               "\"position\":\"unknown\","
               "\"inventory_source_id\": 476,"
               "\"auction_id_64\":8984480746668973511,"
               "\"reserve_price\":1.250,"
               "\"tag_format\":\"javascript\","
               "\"ad_profile_id\":382,"
               "\"default_creative_id\":29,"
               "\"tag_data\":\"page_id=242\","
               "\"querystring\":\"age=21&gender=male\""
            "},"
            "{"
               "\"id\":17,"
               "\"sizes\":[\"300x250\"],"
               "\"position\":\"unknown\","
               "\"inventory_source_id\": 300,"
               "\"auction_id_64\":8984480746668973512,"
               "\"reserve_price\":1.500,"
               "\"tag_format\":\"javascript\","
               "\"ad_profile_id\":389,"
               "\"default_creative_id\":32,"
               "\"tag_data\":\"page_id=242\","
               "\"querystring\":\"age=21&gender=male\""
            "}"
         "],"
         "\"bid_info\":{"
            "\"user_id_64\":2987961585469200312,"
            "\"selling_member_id\":3,"
            "\"no_cookies\":false,"
            "\"ip_address\":\"96.246.152.18\","
            "\"user_agent\":\"Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.5; en-US;rv:1.9.0.3) Gecko/2008092414 Firefox/3.0.3\","
            "\"country\":\"US\","
            "\"region\":\"NY\","
            "\"city\": \"New York\","
            "\"postal_code\": 10014,"
            "\"dma\":501,"
            "\"time_zone\":\"America/New_York\","
            "\"url\":\"http://www.yahoo.com/\","
            "\"domain\":\"yahoo.com\","
            "\"inventory_class\":\"class_1\","
            "\"within_iframe\":false,"
            "\"total_clicks\":0"
         "}"
      "}"
   "}"
};

int main(int argc, char *argv[])
{
   static char top[]=
     "<!DOCTYPE HTML>"
     "<html lang=\"en\"><head><title>Benchmarking JSON</title><meta http-equiv"
     "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
     "<link href=\"/imgs/style.css\" rel=\"stylesheet\" type=\"text/css\">"
     "</head><body style=\"margin:16px;\"><h2>Benchmarking JSON</h2>\r\n";

   xbuf_t *reply = get_reply(argv);
   xbuf_ncat(reply, top, sizeof(top) - 1);

   // -------------------------------------------------------------------------
   // build JSON tree from text, and parse the new JSON tree
   // -------------------------------------------------------------------------
   xbuf_cat(reply, "<b>Build JSON tree from text:</b><br>");
   jsn_t *import = jsn_frtext(s_json, "import"); // import from text
   
   xbuf_xcat(reply, "Parsing %s.<br><br>", import ? "worked" : "failed");

   // -------------------------------------------------------------------------
   // re-display all in compact and formatted form (to check it works)
   // -------------------------------------------------------------------------
   xbuf_cat(reply, "<b>JSON compact output:</b><br>");
   xbuf_t xbuf;
   xbuf_init(&xbuf);
   char *text = jsn_totext(&xbuf, import, 0); // 0:compact form
   xbuf_xcat(reply, "<pre>%s</pre>", text);
   //xbuf_empty(&xbuf);
   //text = jsn_totext(&xbuf, import, 1); // 1:formatted form
   //xbuf_xcat(reply, "<pre>%s</pre>", text);
   xbuf_free(&xbuf); // free the xbuffer supplying memory for the 'text' ptr
   
   // -------------------------------------------------------------------------
   // free the JSON tree
   // -------------------------------------------------------------------------
   xbuf_cat(reply, "<br><b>Freeing the JSON tree:</b><br>");
   jsn_free(import);
   xbuf_cat(reply, "Done.<br><br><br>");

   // =========================================================================
   // now do the same thing repeatedly amd time it
   // -------------------------------------------------------------------------
   {
      #define NBR_ROUNDS 100000
      u64 start = getms();
      int rounds = NBR_ROUNDS;
      while(rounds--)
      {
         import = jsn_frtext(s_json, "import"); // import from text
         jsn_free(import);
      }
      start = getms() - start;
      xbuf_xcat(reply, "<b>Benchmarking JSON</b><br>"
                "On 1 single CPU Core, parsing this JSON tree %U times took"
                " %llU ms<br><br>", NBR_ROUNDS, start);
   }
   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
