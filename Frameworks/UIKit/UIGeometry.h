#import <CoreGraphics/CGGeometry.h>
#import <CoreGraphics/CGAffineTransform.h>

typedef NSUInteger UIRectEdge;

enum {
    UIRectEdgeNone   = 0,
    UIRectEdgeTop    = 1 << 0,
    UIRectEdgeLeft   = 1 << 1,
    UIRectEdgeBottom = 1 << 2,
    UIRectEdgeRight  = 1 << 3,
    UIRectEdgeAll    = UIRectEdgeTop | UIRectEdgeLeft | UIRectEdgeBottom | UIRectEdgeRight,
};

typedef NSInteger UISemanticContentAttribute;

enum {
    UISemanticContentAttributeUnspecified = 0,
    UISemanticContentAttributePlayback,
    UISemanticContentAttributeSpatial,
    UISemanticContentAttributeForceLeftToRight,
    UISemanticContentAttributeForceRightToLeft,
};

typedef NSInteger UIUserInterfaceLayoutDirection;

enum {
    UIUserInterfaceLayoutDirectionLeftToRight = 0,
    UIUserInterfaceLayoutDirectionRightToLeft,
};

typedef NSInteger UIUserInterfaceSizeClass;

enum {
    UIUserInterfaceSizeClassUnspecified = 0,
    UIUserInterfaceSizeClassCompact,
    UIUserInterfaceSizeClassRegular,
};

typedef NSInteger UIBarMetrics;

enum {
    UIBarMetricsDefault,
    UIBarMetricsCompact,
    UIBarMetricsDefaultPrompt = 101,
    UIBarMetricsCompactPrompt,
};

typedef NSInteger UIBarPosition;

enum {
    UIBarPositionAny = 0,
    UIBarPositionBottom,
    UIBarPositionTop,
    UIBarPositionTopAttached,
};

struct UIOffset {
    CGFloat horizontal;
    CGFloat vertical;
};
typedef struct UIOffset UIOffset;

static inline UIOffset UIOffsetMake(CGFloat horizontal, CGFloat vertical) {
    UIOffset offset = {horizontal, vertical};
    return offset;
}

static inline BOOL UIOffsetEqualToOffset(UIOffset a, UIOffset b) {
    return a.horizontal == b.horizontal && a.vertical == b.vertical;
}

UIKIT_EXPORT const UIOffset UIOffsetZero;

struct UIEdgeInsets {
    CGFloat top;
    CGFloat left;
    CGFloat bottom;
    CGFloat right;
};
typedef struct UIEdgeInsets UIEdgeInsets;

static inline UIEdgeInsets UIEdgeInsetsMake(CGFloat top, CGFloat left, CGFloat bottom, CGFloat right) {
    UIEdgeInsets insets = {top, left, bottom, right};
    return insets;
}

static inline BOOL UIEdgeInsetsEqualToEdgeInsets(UIEdgeInsets a, UIEdgeInsets b) {
    return a.top == b.top && a.left == b.left && a.bottom == b.bottom && a.right == b.right;
}

static inline CGRect UIEdgeInsetsInsetRect(CGRect rect, UIEdgeInsets insets) {
    rect.origin.x    += insets.left;
    rect.origin.y    += insets.top;
    rect.size.width  -= (insets.left + insets.right);
    rect.size.height -= (insets.top + insets.bottom);
    return rect;
}

UIKIT_EXPORT const UIEdgeInsets UIEdgeInsetsZero;

typedef NSInteger UILayoutConstraintAxis;

enum {
    UILayoutConstraintAxisHorizontal = 0,
    UILayoutConstraintAxisVertical = 1,
};
