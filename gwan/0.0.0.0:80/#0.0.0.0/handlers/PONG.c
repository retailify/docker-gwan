// ============================================================================
// Protocol handler example for the G-WAN web app. server, see http://gwan.com/
// ----------------------------------------------------------------------------
// PONG.c: implementing a G-WAN (multithreaded and event-based) custom server
//         with a basis HTTP support (no request parsing nor HTTP keep-alives).
//
//   Test with: GET "192.168.2.17:8080/123"
//
//   Test with: weighttp -n1000000 -c300 -t6 "http://192.168.2.7:8080/123"
//              finished in 9 sec, 283 ms and 295 microsec, 
//              107720 req/s, 
//                9152 kbyte/s
//              requests: 1000000 total, 1000000 started, 1000000 done, 
//                        1000000 succeeded, 0 failed, 0 errored
//              status codes: 1000000 2xx, 0 3xx, 0 4xx, 0 5xx
//              traffic: 87000000 bytes total, 
//                       82000000 bytes http, 
//                        5000000 bytes data
//
//   "Protocol handlers" let you re-use the G-WAN server to implement other
//   protocols than HTTP (SSL/TLS, SDPY, websockets, mail, LDAP, DB/KV...). 
//
//   Here, G-WAN is acting as a TCP server (accepting/closing connections, 
//   handling I/O events, allocating/freeing connection contexts, providing
//   a rich API. etc.) while protocol handlers focus on the server purpose:
//   timeouts, log files, request processing, reply construction.
//
//   This instantly gives your drafts/projects the best available multicore
//   performance and the smallest memory footprint. You save on development
//   costs, shorten time-to-market, and can use only one single process on a
//   computer to do many different things with G-WAN protocols/applications.
//
//   Like "Connection handlers" or "Content-type handlers", "Protocol handlers"
//   are stored under the gwan/listener/host directory, under a user-defined 
//   file name (i.e. "PONG.c" or "PONG.cpp" for the "PONG" protocol) which 
//   MUST use uppercase letters.
//
//   If a listener directory structure specifies a protocol (see below) then
//   the corresponding protocol handler script MUST be present so it can be
//   loaded, compiled and executed to handle all the incoming connections of
//   this listener:
//                          protocol                        protocol handler
//               listener     +--+  host (IPv4/DNS)         (uppercase)
//          +---------------+ |  |   +-----------+              +--+
//          |               | |  |   |           |              |  |
//          :               : :  :   :           :              :  :
//   gwan / 192.168.2.17_8080_PONG / #192.168.2.17 / handlers / PONG.c
//          :            : :  :      ^               
//          |            | |  |      |
//          +------------+ +--+      |
//               IPv4      port     root (not meaningful here)
//
// When used as the base listener's protocol like above, protocol handlers do 
// not use /csp servlets, nor the /www folder because handlers are supposed to
// do the work themselves.
//
// But protocol handlers can also be used from regular listeners (using G-WAN's
// HTTP default protocol) to let other handlers or servlets launch or process
// connections to another host via different protocols, making it easier to use
// proxy or backend servers, synchronization hosts, etc.
//
// Used this way, protocol handlers also let you define asynchronous database,
// mail or LDAP clients in a centralized way insted of using a pre-compiled
// library (less practical to use the G-WAN API) or source code duplicated 
// wherever you need to invoke these client capabilities.
// ============================================================================
// The define below is a development helper, giving at least the handler faulty
// line number in crash reports, even if the stack frames are corrupted.
// (comment the define for production: it disables all optimizations)
#pragma debug 
// ----------------------------------------------------------------------------

#include "gwan.h" // the G-WAN API

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

// demonstrating how to define your (optional) connection context
// (G-WAN will allocate/free it dynamically for all connections)
#define USE_CUSTOM_CONNECTION_CONTEXT

// enable/disable tracing in terminal (useful for debug/performance test)
//#define putsx printf     // debug/trace
#define putsx            // performance test

// ============================================================================
// setup any 'global' settings you need here (configuration, buffers, etc.)
// ----------------------------------------------------------------------------
#ifdef USE_CUSTOM_CONNECTION_CONTEXT
   typedef struct
   {
      s64 birth; // time (micro-sec) at which we received our first connection
      s64 time;  // time (micro-sec) before we received the client request
   } my_ctx_t;

   int init(int argc, char *argv[]) 
   { 
      // tell G-WAN what memory space our custom connection context needs
      u32 *ctx_size = (u32*)get_env(argv, US_HANDLER_CTX_SIZE);
      *ctx_size = sizeof(my_ctx_t);
      return 0; 
   }
#else // if you don't need any context, just return 0
   int init(int argc, char *argv[]) { return 0; }
#endif

// ----------------------------------------------------------------------------
// save/free 'global' settings here (configuration, buffers, etc.)
// ----------------------------------------------------------------------------
void clean(int argc, char *argv[]) { }

// ---------------------------------------------------------------------
// Protocol states (32 maximum):
// ---------------------------------------------------------------------
// C->state is user-defined (G-WAN will not setup nor modify it)
// so let's define our custom protocol states:
enum PROTOCOL_events
{
   PRT_FREE,     // unused slot
   PRT_ACCEPTED, // a new connection was accepted
   PRT_READING,  // data can be fetched from the input socket buffer
   PRT_SENDING,  // data can be added to the output socket buffer
   PRT_CLOSING,  // the connection was closed (client close or error)
};
static const char *s_states[] =
{
   [PRT_FREE]     = "FREE    ",
   [PRT_ACCEPTED] = "ACCEPTED",
   [PRT_READING]  = "READING ",
   [PRT_SENDING]  = "SENDING ",
   [PRT_CLOSING]  = "CLOSING ",
};

// ============================================================================
// server worker thread called to process connections
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // ---------------------------------------------------------------------
   // G-WAN connection context:
   // ---------------------------------------------------------------------
   // C->ip.num    // client IP address as a string
   // C->ip.str    // client IP address as an integer
   // C->fd;       // client socket we accepted from client
   // C->id        // fd initial value, even after close()
   // C->state     // connection state (user-defined)
   // C->accepted  // a new connection has been accepted by the server
   // C->connected // a connect() call initiated by you has succeeded
   // C->can_recv  // data is available in socket buffer
   // C->can_send  // free room is available in socket buffer
   // C->closing   // connection closed by client or error
   // C->ctx       // user-defined protocol-handler context (optional)
   // ---------------------------------------------------------------------
   conn_t *C = (void*)argv[0]; // G-WAN connection context

#ifdef USE_CUSTOM_CONNECTION_CONTEXT
   my_ctx_t *c = (void*)C->ctx; // user-defined connection context
#endif

   // ---------------------------------------------------------------------
   // Connection events:
   // ---------------------------------------------------------------------
   // C->accepted and C->connected are persistent value setup by G-WAN to
   // allow 'Protocol Handlers' distinguish 'accepted' from 'connected'
   // connections. Other events are updated dynamically.
   // ---------------------------------------------------------------------
   // set the start/end states by using asynchronous G-WAN events
   if(C->state == PRT_FREE && C->accepted) C->state = PRT_ACCEPTED; else
   if(C->closing)                          C->state = PRT_CLOSING;

   putsx("%s accepted:%d connected:%d can_recv:%d can_send:%d closing:%d\n", 
          s_states[C->state],
          C->accepted, C->connected, C->can_recv, C->can_send, C->closing);

   // output example explained:
   // ---------------------------------------------------------------------
   // ACCEPTED accepted:1 connected:0 can_recv:0 can_send:0 closing:0
   //          fd:5] ip:'192.168.2.7'
   // WAIT_REQ accepted:1 connected:0 can_recv:0 can_send:0 closing:0
   // WAIT_REQ accepted:1 connected:0 can_recv:0 can_send:1 closing:0
   // WAIT_REQ accepted:1 connected:0 can_recv:1 can_send:1 closing:0
   //          fd:5] recv:147 bytes after 11 us
   //          fd:5] sent:4 bytes
   // CLOSING  accepted:1 connected:0 can_recv:0 can_send:0 closing:1
   //          fd:5] ip:'192.168.2.7'
   // ---------------------------------------------------------------------
   // The WAIT_REQ state is listed several times because there was a delay
   // between the ACCEPTED state and the incoming client request. Receiving
   // several notifications for the 'accepted' state lets you decide at
   // at which point you have been waiting too long for the client to send
   // a request. At this time, you might decide to close the connection.
   //
   // The SENDING state is not listed above because (a) C->can_send was
   // 'true' after recv() so we first skipped WAIT_SEND and then (b) our
   // send() call didn't fill the socket buffer so all was sent in one shot.
             
   switch(C->state)
   {
      // we are told that a new connection has been accepted by the server
      // ----------------------------------------------------------------------
      case PRT_ACCEPTED:
      {
#ifdef USE_CUSTOM_CONNECTION_CONTEXT
         const s64 now = getus(); // microseconds
         if(!c->birth) c->birth = now;
         c->time = now;
#endif
         C->state = PRT_READING;
         putsx("          fd:%2d] ip:'%s'\n", C->fd, C->ip.str);
         break;
      }
      // ----------------------------------------------------------------------
      case PRT_READING:
      {
         // are we told that data can be read from the system TCP input buffer?
         if(!C->can_recv) 
            break; // the input socket buffer is empty, wait for data to come
         
#ifdef USE_CUSTOM_CONNECTION_CONTEXT
         c->time = getus() - c->time;
#endif
         char buf[1024] = {0};
         int bytes = 0;
         for(;;)
         {
            const int ret = recv(C->fd, &buf, sizeof(buf), 0);
            if(ret <= 0) // closed, no more bytes, errors...
               break;
            bytes += ret;
         }
         
         C->state = PRT_SENDING;

#ifdef USE_CUSTOM_CONNECTION_CONTEXT
         putsx("          fd:%2d] recv:%d bytes after %ld us\n", 
                C->fd, bytes, c->time);
#else
         putsx("          fd:%2d] recv:%d bytes\n", C->fd, bytes);
#endif
         // we don't break to send immediately if possible
         //break;
      }
      // ----------------------------------------------------------------------
      case PRT_SENDING:
      {
         // are we told that data can be sent to the system TCP output buffer?
         if(!C->can_send) 
            break; // the output socket buffer is full, wait for free room

         static const char reply[] = "HTTP/1.1 200 OK\r\n"
                                       "Content-Length: 5\r\n"
                                       "Content-Type: text/html\r\n"
                                       "Connection: close\r\n\r\n"
                                       "PONG\n";
         int bytes = send(C->fd, reply, sizeof(reply) - 1, 0);
         putsx("          fd:%2d] sent:%d bytes\n", C->fd, bytes);

         // if we implemented a keep-alive protocol like HTTP/1.1 
         // we would 'break;' and process another requests but
         // PONG.c (gracefully) closes connections after a request:
         // break;
         return 0; // close this connection
      }
      // we are told just before cleanups that the connection will be closed
      // ----------------------------------------------------------------------
      case PRT_CLOSING:
      {
         putsx("          fd:%2d] ip:'%s'\n\n", C->fd, C->ip.str);
         break;
      }
   }

   return 1; // 1:continue, 0:gracefully close connection
}
// ============================================================================
// End of Source Code
// ============================================================================

