
static NSMutableDictionary *instancesDict;
static NSMutableArray *liveInstances;


@interface NSObject(BundleReferences)
// external references
- (NSArray *)getNSArray;
- (NSArray *)subviews;
- (id)contentView;
- (id)document;
- (id)delegate;
- (SEL)action;
- (id)target;
@end


@interface NSObject(SharedInstances)
+ (id)sharedInstance;
@end


@interface BundleInjection(Sweeper)
+ (NSMutableDictionary *)instancesSeen;
+ (NSMutableArray *)liveInstances;
@end


@implementation BundleInjection(Sweeper)

+ (void)addMethod {
    // add bsweep method to SwiftObject class!
    Class swiftRoot = objc_getClass( "SwiftObject" );
    SEL methodSEL = @selector(bsweep);
    Method method = class_getInstanceMethod([NSObject class], methodSEL);
    int = class_addMethod( swiftRoot, methodSEL,
                          method_getImplementation( method ),
                          method_getTypeEncoding( method ) );
    if ( !result )
        NSLog( @"BundleSweeper: Could not add SwiftObject method: %s %p %s", sel_getName(methodSEL),
              (void *)method_getImplementation( method ), method_getTypeEncoding( method ) );
}

+ (NSArray *)sweepForLiveObjects {
    static int setup;
    if ( !setup++ ) {
        [self addMethod];
    }

    Class bundleInjection = objc_getClass("BundleInjection");
    bundleInjection.instancesSeen = [NSMutableDictionary new];
    bundleInjection.liveInstances = [NSMutableArray new];

    //NSTimeInterval start = [NSDate timeIntervalSinceReferenceDate];
    [[self bprobeSeeds] bsweep];
    //NSLog( @"%f", [NSDate timeIntervalSinceReferenceDate]-start );

    NSArray *liveInstances = bundleInjection.liveInstances;
    bundleInjection.instancesSeen = nil;
    bundleInjection.liveInstances = nil;
    return liveInstances;
}

+ (void)setupsharedInstances {
    sharedInstances = [NSMutableArray new];

    unsigned nc;
    Class *classes = objc_copyClassList( &nc );
    for ( unsigned i=0 ; i<nc ; i++ ) {
        Method m = class_getInstanceMethod( object_getClass( classes[i] ), @selector(sharedInstance) );
        if (!m) continue;

        Dl_info info;
        dladdr( (__bridge const void *)classes[i], &info );

#if TARGET_IPHONE_SIMULATOR
        static char userClass[] = "/Users/";
#else
        static char userClass[] = "/private/var/";
#endif
        if ( strncmp( info.dli_fname, userClass, sizeof userClass-1 ) == 0 &&
            strstr( info.dli_fname, "/InjectionBundle" ) == 0 ) {
            id (*imp)( Class, SEL ) = (id (*)( Class, SEL ))method_getImplementation( m );
            NSLog( @"BundleSweeper: +[%@ sharedInstance] == %p()", classes[i], (void *)imp );
            id shared = imp( classes[i], @selector(sharedInstance) );
            if ( shared )
                [sharedInstances addObject:shared];
        }

    }

    free(classes);
}
+ (NSArray *)sweepSharedInstances {
    if ( !sharedInstances ) {
        [self setupsharedInstances];
    }

    return [self sweepForLiveObjects];
}


+ (NSMutableDictionary *)instancesSeen {
    return instancesDict;
}

+ (void)setInstancesSeen:(NSMutableDictionary *)dictionary {
    instancesDict = dictionary;
}

+ (NSMutableArray *)liveInstances {
    return liveInstances;
}

+ (void)setLiveInstances:(NSMutableArray *)array {
    liveInstances = array;
}


@end
