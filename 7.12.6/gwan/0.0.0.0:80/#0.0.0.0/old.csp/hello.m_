// ============================================================================
// Objective-C servlet example for the G-WAN Web App. Server  (http://gwan.ch/)
// ----------------------------------------------------------------------------
// hello.m: the real stuff rather than the mere C-compatibility mode
//          (I initially wrote hello.m with #import instead of #include)
//
// This code was submitted and written by Thomas Meitz (gwan at jproxx dot com)
// ============================================================================
// imported functions:
//   get_reply(): get a pointer on the 'reply' dynamic buffer from the server
//    xbuf_cat(): like strcat(), but it works in the specified dynamic buffer
// ----------------------------------------------------------------------------
#import "gwan.h" // G-WAN exported functions
#import <objc/Object.h>

@interface Hello : Object {
        xbuf_t *replyBuffer;
}
- (Hello *) initWithBuffer:(xbuf_t *) reply;
- (void) whatsup;
@end

@implementation Hello 
- (Hello *) initWithBuffer:(xbuf_t *) reply
{
        replyBuffer = reply;
        return self;
}
- (void) whatsup
{
        xbuf_cat(replyBuffer, "Hello World (Objective-C)");
}
@end
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   Hello *hello = [[Hello alloc] initWithBuffer:get_reply(argv)];
   [hello whatsup];
   [hello free];
   return 200; // return an HTTP code (200:'OK')
}
// ============================================================================
// End of Source Code
// ============================================================================

