

@implementation NSObject(BundleSweeper)
+ (void)bsweep {}

- (void)bsweep {
    id key = [NSValue valueWithPointer:(__bridge void *)self];
    Class bundleInjection = objc_getClass("BundleInjection");
    if ( [bundleInjection instancesSeen][key] )
        return;

    [bundleInjection instancesSeen][key] = @"1";

    Class aClass = object_getClass(self);
    NSString *className = NSStringFromClass(aClass);
    if ( [className characterAtIndex:1] == '_' || [className hasPrefix:@"UITransition"] )
        return;

    [[bundleInjection liveInstances] addObject:self];

    //printf("BundleSweeper sweep <%s %p>\n", [className UTF8String], self);

    for ( ; aClass && aClass != [NSObject class] ; aClass = class_getSuperclass(aClass) ) {
        static Class xprobeSwift;
        if ( isSwift( aClass ) && (xprobeSwift = xprobeSwift ?: xloadXprobeSwift("Xprobe")) ) {
            [xprobeSwift injectionSweep:self forClass:aClass];
            continue;
        }

        unsigned ic;
        Ivar *ivars = class_copyIvarList(aClass, &ic);
        const char *currentClassName = class_getName(aClass), firstChar = currentClassName[0];

        if ( firstChar != '_' && !(firstChar == 'N' && currentClassName[1] == 'S') &&
            strncmp( currentClassName, "RAC", 3 ) != 0 ) // uses unsafe_unretained
            for ( unsigned i=0 ; i<ic ; i++ ) {
                __unused const char *currentIvarName = ivar_getName(ivars[i]);
                const char *type = ivar_getTypeEncodingSwift( ivars[i], aClass );
                if ( type && (type[0] == '@' || isSwiftObject( type )) ) {
                    id subObject = xvalueForIvarType( self, ivars[i], type, aClass );
                    if ( [subObject respondsToSelector:@selector(bsweep)] )
                        [subObject bsweep];////( subObject );
                }
            }

        free( ivars );
    }

    if ( [self respondsToSelector:@selector(target)] )
        [[self target] bsweep];
    if ( [self respondsToSelector:@selector(delegate)] )
        [[self delegate] bsweep];
    if ( [self respondsToSelector:@selector(document)] )
        [[self document] bsweep];

    if ( [self respondsToSelector:@selector(contentView)] &&
        [[self contentView] respondsToSelector:@selector(superview)] )
        [[[self contentView] superview] bsweep];
    if ( [self respondsToSelector:@selector(subviews)] )
        [[self subviews] bsweep];
    if ( [self respondsToSelector:@selector(getNSArray)] )
        [[self getNSArray] bsweep];
}
@end


@implementation NSArray(BundleSweeper)
- (void)bsweep {
    for ( id obj in self )
        if ( [obj respondsToSelector:@selector(bsweep)] )
            [obj bsweep];////( subObject );
}
@end

@implementation NSSet(BundleSweeper)
- (void)bsweep {
    [[self allObjects] bsweep];
}
@end

@implementation NSDictionary(BundleSweeper)
- (void)bsweep {
    [[self allValues] bsweep];
}
@end

@implementation NSMapTable(BundleSweeper)
- (void)bsweep {
    [[[self objectEnumerator] allObjects] bsweep];
}
@end

@implementation NSHashTable(BundleSweeper)
- (void)bsweep {
    [[self allObjects] bsweep];
}
@end

@implementation NSString(BundleSweeper)
- (void)bsweep {}
@end

@implementation NSValue(BundleSweeper)
- (void)bsweep {}
@end

@implementation NSData(BundleSweeper)
- (void)bsweep {}
@end

@implementation NSBlock(BundleSweeper)
- (void)bsweep {}
@end

