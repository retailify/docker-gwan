// ============================================================================
// C servlet sample for the G-WAN Web Application Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// cookies.c: list cookies, if any
//
// created by Eric Schulz (Ers35)
// ----------------------------------------------------------------------------
#include "gwan.h"
#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------------------
// where 'cookies' = "key1=value1; key2=value2;"
// ----------------------------------------------------------------------------
static kv_t parse_cookies(char *cookies)
{
  kv_t cookies_store;
  kv_init(&cookies_store, "cookies", 1024, 0, 0, 0);
  
  char *key, *val, *lasts = 0;
  for(;;)
  {
     key = strtok_r(cookies, "= ", &lasts);
     val = strtok_r(0, ";,", &lasts);
    
     if(!val) break; //no more cookies
    
     kv_add(&cookies_store, 
            &(kv_item){
             .key = key,
             .val = val,
             .flags = 0,
           });
    
     cookies = 0;
  }
  return cookies_store;
}
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // using the client cookies (make sure that they are there)
  // http_t *http = get_env(argv, HTTP_HEADERS, 0);
  // kv_t cookies_store = parse_cookies(http->h_cookies);

  // using fixed cookies (for tests without client cookies)
  char cookies[] = "key=val;foo=bar"; 
  kv_t cookies_store = parse_cookies(cookies);
  
  char *val = kv_get(&cookies_store, "key", sizeof("key") - 1);
  
  printf("%s = %s\n", "key", val);
  
  kv_free(&cookies_store);
  return 204; // empty reply
}
// ============================================================================
// End of Source Code
// ============================================================================

