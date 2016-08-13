// ============================================================================
//forum example to demonstrate G-WAN's API
//http://forum.gwan.com/index.php?p=/discussion/470/forum-software-written-in-c/
//
// Created by Eric Schulz (Ers35)
//------------------------------------------------------------------------------

//include required headers------------------------------------------------------
#include "gwan.h"

#include <malloc.h>
#include <stdlib.h>
//------------------------------------------------------------------------------

//data structures---------------------------------------------------------------
typedef struct {
  u64 id;
  xbuf_t body;
} Post;

typedef struct {
  u64 id;
  xbuf_t title;
  kv_t posts;
} Thread;
//------------------------------------------------------------------------------

//HTML templates----------------------------------------------------------------
static char 
  base_tpl[] = 
    "<html><body>"
    "<head><title>G-WAN Forum</title></head>"
    "<h1><a href='/?forum_simple/'>G-WAN Forum</a></h1><hr/>"
    "<form method='POST'><!--form--></form>"
    "<!--tpl-->"
    "</body></html>",
  post_form[] = 
    "<h2><!--title--></h2>" //title of thread
    "<input name='id' type='hidden' value='<!--id-->' />" //thread id
    "<input name='act' type='hidden' value='p' />" //new post
    "<textarea name='body'>This is a post.</textarea><br/>"
    "<input type='submit' value='Create post'/>",
  thread_form[] = 
    "<input name='act' type='hidden' value='t' />" //new thread
    "<input type='text' name='title' value='Thread title'></input>"
    "<input type='submit' value='Create thread'/>";
//------------------------------------------------------------------------------

//template rendering functions--------------------------------------------------
int list_posts(kv_item *item, xbuf_t *reply)
{
  Post *post = (Post*)item->val;
  
  //using HTML comments
  const char post_li[] = "<li><!--contents--></li>";
  
  char *pos = (char*)xbuf_findstr(reply, "<!--tpl-->");
  if (pos) xbuf_insert(reply, pos, sizeof(post_li), (char*)post_li);
  
  xbuf_repl(reply, "<!--contents-->", post->body.ptr);
    
  return 1; //continue searching
}

int list_threads(kv_item *item, xbuf_t *reply)
{
  Thread *thread = (Thread*)item->val;
  
  xbuf_t thread_li;
  xbuf_init(&thread_li);
  
  //using sprintf-like formatting
  xbuf_xcat(&thread_li, 
    "<li>"
    "<a href='/?forum_simple/act=t/id=%llu'>%s</a> (%lu)"
    "</li>",
    thread->id, thread->title.ptr, thread->posts.nbr_items
  );
  
  char *pos = (char*)xbuf_findstr(reply, "<!--tpl-->");
  if (pos) xbuf_insert(reply, pos, thread_li.len, thread_li.ptr);

  xbuf_free(&thread_li);

  return 1;
}
//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  //initialize Key-Value store--------------------------------------------------
  kv_t **vhost_ptr = (kv_t**)get_env(argv, US_VHOST_DATA), //persistent ptr
       *forum_store = 0; //convenience pointer (var->m instead of (*var)->m)
  
  if (vhost_ptr && !*vhost_ptr)
  {
    *vhost_ptr = (kv_t*)malloc(sizeof(kv_t));
    
    //threads and posts stored here
    kv_init(*vhost_ptr, "forum_store", 1024, 0, 0, 0);
  } forum_store = *vhost_ptr;
  //----------------------------------------------------------------------------
  
  //setup GET and POST variables------------------------------------------------
  char *act = "", *id = "", *title = "", *body = "";
  get_arg("act=",   &act,   argc, argv); //action ('t' or 'p')
  get_arg("id=",    &id,    argc, argv); //id of thread
  get_arg("title=", &title, argc, argv); //title of thread
  get_arg("body=",  &body,  argc, argv); //body of post
  
  char *end = 0;
  u64 int_id = strtoll(id, &end, 10); //string to integer
  if (*end) int_id = 0; //require a numeric ID
  //----------------------------------------------------------------------------
  
  //response sent to browser is stored here-------------------------------------
  //templates are rendered into this buffer
  xbuf_t *reply = get_reply(argv);
  xbuf_cat(reply, base_tpl); //set base template
  //----------------------------------------------------------------------------
  
  //HTTP state of a connection
  http_t *http = (http_t*)get_env(argv, HTTP_HEADERS);
  
  redirect: //simulate HTTP, <meta>, or JavaScript redirect without page reload
  
  //choose what to do based on the value of 'act'-------------------------------
  switch (*act)
  {
    //regarding a post
    case 'p':
    {
      switch (http->h_method) //GET or POST
      {
        //new post--------------------------------------------------------------
        case HTTP_POST:
        {
          //get the thread to which this post belongs
          Thread *thread = (Thread*)kv_get(forum_store, (char*)&int_id, sizeof(int_id));
          
          if (!thread) //thread not found
          {
            xbuf_repl(reply, "<!--tpl-->", http_error(404));
            return HTTP_404_NOT_FOUND;
          }
          
          //allocate memory
          Post *post = calloc(1, sizeof(*post));
          
          //initialize members
          xbuf_init(&post->body);
          
          //define members
          post->id = thread->posts.nbr_items + 1;
          xbuf_cat(&post->body, *body ? body : " ");
          
          //add post to thread
          kv_add(&thread->posts, &(kv_item) {
            .key = (char*)&post->id,
            .klen = sizeof(post->id),
            .val = (char*)post,
            .in_use = 0,
          });
          
          //setup redirect
          http->h_method = HTTP_GET;
          *act = 't';
          
          goto redirect;
        } break;
        //----------------------------------------------------------------------
      }
    } break;
    
    //regarding a thread
    case 't':
    {
      switch (http->h_method)
      {
        //view a thread---------------------------------------------------------
        case HTTP_GET:
        {
          Thread *thread = (Thread*)kv_get(forum_store, (char*)&int_id, sizeof(int_id));
          
          if (!thread)
          {
            xbuf_repl(reply, "<!--tpl-->", http_error(404));
            return HTTP_404_NOT_FOUND;
          }
          
          //replace template variables with dynamic values
          xbuf_repl(reply, "<!--form-->", post_form);
          xbuf_repl(reply, "<!--title-->", thread->title.ptr);
          xbuf_repl(reply, "<!--id-->", id);
          
          //for each post, render its template and insert it into reply
          kv_do(&thread->posts, 0, 0, (kv_proc_t)&list_posts, (void*)reply);
        } break;
        //----------------------------------------------------------------------
        
        //create a thread-------------------------------------------------------
        case HTTP_POST:
        {
          Thread *thread = calloc(1, sizeof(*thread));
          
          xbuf_init(&thread->title);
          kv_init(&thread->posts, "posts", 1024, 0, 0, 0);
          
          thread->id = forum_store->nbr_items + 1;
          xbuf_cat(&thread->title, *title ? title : " ");
          
          //add thread to KV store
          kv_add(forum_store, &(kv_item) {
            .key = (char*)&thread->id,
            .klen = sizeof(thread->id),
            .val = (char*)thread,
            .in_use = 0,
          });
          
          http->h_method = HTTP_GET;
          *act = 0;
          
          goto redirect;
        } break;
        //----------------------------------------------------------------------
      }
    } break;
    
    //list all threads----------------------------------------------------------
    default:
    {      
      xbuf_repl(reply, "<!--form-->", thread_form);
      
      kv_do(forum_store, 0, 0, (kv_proc_t)&list_threads, reply);
    } break;
    //--------------------------------------------------------------------------
  }
  //----------------------------------------------------------------------------
  
  return HTTP_200_OK; //HTTP code 200, no errors, headers generated automatically
}

