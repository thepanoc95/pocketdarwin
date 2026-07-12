#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>

typedef uint64_t UIAccessibilityTraits;

UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitNone;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitButton;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitLink;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitHeader;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitSearchField;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitImage;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitSelected;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitPlaysSound;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitKeyboardKey;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitStaticText;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitSummaryElement;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitNotEnabled;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitUpdatesFrequently;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitStartsMediaSession;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitAdjustable;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitAllowsDirectInteraction;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitCausesPageTurn;
UIKIT_EXPORT UIAccessibilityTraits UIAccessibilityTraitTabBar;

@interface NSObject (UIAccessibility)

@property (nonatomic, assign) BOOL isAccessibilityElement;
@property (nonatomic, retain) NSString *accessibilityLabel;
@property (nonatomic, retain) NSString *accessibilityHint;
@property (nonatomic, retain) NSString *accessibilityValue;
@property (nonatomic, assign) UIAccessibilityTraits accessibilityTraits;
@property (nonatomic, assign) CGRect accessibilityFrame;
@property (nonatomic, retain) UIBezierPath *accessibilityPath;
@property (nonatomic, retain) NSString *accessibilityIdentifier;

@property (nonatomic, assign) NSInteger accessibilityElementCount;
@property (nonatomic, assign) NSInteger accessibilityElementAtIndex:(NSInteger)index;
@property (nonatomic, assign) NSInteger indexOfAccessibilityElement:(id)element;

@property (nonatomic, retain) NSArray *accessibilityElements;
@property (nonatomic, retain) UIView *accessibilityContainer;

@property (nonatomic, retain) NSString *accessibilityLanguage;
@property (nonatomic, assign) BOOL accessibilityElementsHidden;
@property (nonatomic, assign) BOOL accessibilityViewIsModal;
@property (nonatomic, assign) BOOL shouldGroupAccessibilityChildren;

@property (nonatomic, retain) NSString *accessibilityCustomActions;

- (BOOL)accessibilityActivate;
- (CGPoint)accessibilityActivationPoint;

@end

@interface NSObject (UIAccessibilityAction)

- (BOOL)accessibilityPerformEscape;
- (BOOL)accessibilityPerformMagicTap;

@end

@interface UIAccessibilityElement : NSObject

- (instancetype)initWithAccessibilityContainer:(id)container;

@property (nonatomic, assign) id accessibilityContainer;
@property (nonatomic, assign) BOOL isAccessibilityElement;
@property (nonatomic, retain) NSString *accessibilityLabel;
@property (nonatomic, retain) NSString *accessibilityHint;
@property (nonatomic, retain) NSString *accessibilityValue;
@property (nonatomic, assign) UIAccessibilityTraits accessibilityTraits;
@property (nonatomic, assign) CGRect accessibilityFrame;
@property (nonatomic, retain) UIBezierPath *accessibilityPath;
@property (nonatomic, retain) NSString *accessibilityIdentifier;

@end

typedef NSInteger UIAccessibilityNavigationStyle;

enum {
    UIAccessibilityNavigationStyleAutomatic,
    UIAccessibilityNavigationStyleSeparate,
    UIAccessibilityNavigationStyleCombined,
};
