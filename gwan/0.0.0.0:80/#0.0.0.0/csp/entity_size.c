// ============================================================================
// C servlet example for the G-WAN Web Application Server (http://www.gwan.ch/)
// ----------------------------------------------------------------------------
// entity_size.c: how to modify the maximum HTTP entity size accepted by G-WAN
//
//           Many production servers disable entities to avoid abuse. G-WAN so
//           far used a limit slightly larger than 4KiB.
//
//           This example shows how to modify this (server-global) limit from 
//           a servlet but this can also be done in the init() or the main()
//           calls of a connection handler:
//
//           u32 *max_entity_size = (int*)get_env(argv, MAX_ENTITY_SIZE);
//           *max_entity_size = 200 * 1024; // new size in bytes (200 KiB)
//
//           You can change the limit at any time (even for the time a given 
//           user is connected) by using filtering in a connection handler.
//           Your servlets will decide what to do with the entity anyway so
//           you can dispose or store on disk or do real-time processing, see
//           the entity.c example.
// ============================================================================
#include "gwan.h"   // G-WAN exported functions

int main(int argc, char *argv[])
{
   u32 *old_entity_size = (u32*)get_env(argv, MAX_ENTITY_SIZE);
   u32  new_entity_size = 2 * 1024 * 1024; // 2 MiB
   
   xbuf_xcat(get_reply(argv), 
             "<pre>\n"
             "Old entity size: %llk\n"
             "New entity size: %llk\n" // "New entity size: 2.00 MiB"
             "</pre>",             
             *old_entity_size, new_entity_size);
             
   *old_entity_size = new_entity_size; // raise the limit to 2 MiB
            
   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================

