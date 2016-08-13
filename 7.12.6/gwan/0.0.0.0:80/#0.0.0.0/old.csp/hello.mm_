// ============================================================================
// Objective-C++ script example for the G-WAN Web App Server  (http://gwan.ch/)
// ----------------------------------------------------------------------------
// hello.mm: the real stuff rather than the mere C++-compatibility mode
//           (I initially wrote hello.mm with #import instead of #include)
//
// This code was submitted and written by Thomas Meitz (gwan at jproxx dot com)
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//    xbuf_cat(): like strcat(), but it works in the specified dynamic buffer
// ----------------------------------------------------------------------------
#import "gwan.h" // G-WAN exported functions
#import <objc/Object.h>

@interface Hello_cpp : Object {
        xbuf_t *replyBuffer;
}
- (Hello_cpp *) initWithBuffer:(xbuf_t *) reply;
- (void) whatsup_cpp;
@end

@implementation Hello_cpp
- (Hello_cpp *) initWithBuffer:(xbuf_t *) reply
{
        replyBuffer = reply;
        return self;
}
- (void) whatsup_cpp
{
        xbuf_cat(replyBuffer, (char*)"Hello World (Objective-C++)");
}
@end
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   Hello_cpp *hello = [[Hello_cpp alloc] initWithBuffer:get_reply(argv)];
   [hello whatsup_cpp];
   [hello free];
   return 200; // return an HTTP code (200:'OK')
}
// ============================================================================
// End of Source Code
// ============================================================================

