
#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED

#ifndef XPROBE_BUNDLE
#import <UIKit/UIKit.h>
@interface CCDirector
+ (CCDirector *)sharedDirector;
@end
#endif // XPROBE_BUNDLE

@implementation BundleInjection(BundleSeeds)
+ (NSArray *)bprobeSeeds {
    UIApplication *app = [UIApplication sharedApplication];
    NSMutableArray *seeds = [[app windows] mutableCopy];
    [seeds insertObject:app atIndex:0];
    if ( sharedInstances )
        [seeds addObjectsFromArray:sharedInstances];

    // support for cocos2d
    Class ccDirectorClass = NSClassFromString(@"CCDirector");
    CCDirector *ccDirector = [ccDirectorClass sharedDirector];
    if ( ccDirector )
        [seeds addObject:ccDirector];
    return seeds;
}
@end

#else // __IPHONE_OS_VERSION_MIN_REQUIRED

#import <Cocoa/Cocoa.h>
@implementation BundleInjection(BundleSeeds)
+ (NSArray *)bprobeSeeds {
    NSApplication *app = [NSApplication sharedApplication];
    NSMutableArray *seeds = [[app windows] mutableCopy];
    if ( app.delegate )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullable-to-nonnull-conversion"
        [seeds addObject:app.delegate];
#pragma clang diagnostic pop
    return seeds;
}
@end

#endif // __IPHONE_OS_VERSION_MIN_REQUIRED
