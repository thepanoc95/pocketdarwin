#import <UIKit/UIAccessibility.h>

UIAccessibilityTraits UIAccessibilityTraitNone                = 0;
UIAccessibilityTraits UIAccessibilityTraitButton              = 1 << 0;
UIAccessibilityTraits UIAccessibilityTraitLink                = 1 << 1;
UIAccessibilityTraits UIAccessibilityTraitHeader              = 1 << 2;
UIAccessibilityTraits UIAccessibilityTraitSearchField         = 1 << 3;
UIAccessibilityTraits UIAccessibilityTraitImage               = 1 << 4;
UIAccessibilityTraits UIAccessibilityTraitSelected             = 1 << 5;
UIAccessibilityTraits UIAccessibilityTraitPlaysSound           = 1 << 6;
UIAccessibilityTraits UIAccessibilityTraitKeyboardKey          = 1 << 7;
UIAccessibilityTraits UIAccessibilityTraitStaticText           = 1 << 8;
UIAccessibilityTraits UIAccessibilityTraitSummaryElement       = 1 << 9;
UIAccessibilityTraits UIAccessibilityTraitNotEnabled           = 1 << 10;
UIAccessibilityTraits UIAccessibilityTraitUpdatesFrequently    = 1 << 11;
UIAccessibilityTraits UIAccessibilityTraitStartsMediaSession   = 1 << 12;
UIAccessibilityTraits UIAccessibilityTraitAdjustable           = 1 << 13;
UIAccessibilityTraits UIAccessibilityTraitAllowsDirectInteraction = 1 << 14;
UIAccessibilityTraits UIAccessibilityTraitCausesPageTurn       = 1 << 15;
UIAccessibilityTraits UIAccessibilityTraitTabBar               = 1 << 16;

@implementation UIAccessibilityElement

- (instancetype)initWithAccessibilityContainer:(id)container {
    self = [super init];
    if (self) {
        _accessibilityContainer = container;
        _isAccessibilityElement = YES;
        _accessibilityTraits = UIAccessibilityTraitNone;
        _accessibilityFrame = CGRectZero;
    }
    return self;
}

- (id)accessibilityContainer {
    return _accessibilityContainer;
}

- (void)setAccessibilityContainer:(id)container {
    _accessibilityContainer = container;
}

- (BOOL)isAccessibilityElement {
    return _isAccessibilityElement;
}

- (void)setIsAccessibilityElement:(BOOL)element {
    _isAccessibilityElement = element;
}

- (NSString *)accessibilityLabel {
    return _accessibilityLabel;
}

- (void)setAccessibilityLabel:(NSString *)label {
    label = [label copy];
    [_accessibilityLabel release];
    _accessibilityLabel = label;
}

- (NSString *)accessibilityHint {
    return _accessibilityHint;
}

- (void)setAccessibilityHint:(NSString *)hint {
    hint = [hint copy];
    [_accessibilityHint release];
    _accessibilityHint = hint;
}

- (NSString *)accessibilityValue {
    return _accessibilityValue;
}

- (void)setAccessibilityValue:(NSString *)value {
    value = [value copy];
    [_accessibilityValue release];
    _accessibilityValue = value;
}

- (UIAccessibilityTraits)accessibilityTraits {
    return _accessibilityTraits;
}

- (void)setAccessibilityTraits:(UIAccessibilityTraits)traits {
    _accessibilityTraits = traits;
}

- (CGRect)accessibilityFrame {
    return _accessibilityFrame;
}

- (void)setAccessibilityFrame:(CGRect)frame {
    _accessibilityFrame = frame;
}

- (UIBezierPath *)accessibilityPath {
    return nil;
}

- (void)setAccessibilityPath:(UIBezierPath *)path {
}

- (NSString *)accessibilityIdentifier {
    return _accessibilityIdentifier;
}

- (void)setAccessibilityIdentifier:(NSString *)identifier {
    identifier = [identifier copy];
    [_accessibilityIdentifier release];
    _accessibilityIdentifier = identifier;
}

- (void)dealloc {
    [_accessibilityLabel release];
    [_accessibilityHint release];
    [_accessibilityValue release];
    [_accessibilityIdentifier release];
    [super dealloc];
}

@end
