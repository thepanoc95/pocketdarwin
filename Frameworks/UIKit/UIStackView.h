#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>

typedef NSInteger UIStackViewDistribution;

enum {
    UIStackViewDistributionFill = 0,
    UIStackViewDistributionFillEqually,
    UIStackViewDistributionFillProportionally,
    UIStackViewDistributionEqualSpacing,
    UIStackViewDistributionEqualCentering,
};

typedef NSInteger UIStackViewAlignment;

enum {
    UIStackViewAlignmentFill,
    UIStackViewAlignmentLeading,
    UIStackViewAlignmentTop = UIStackViewAlignmentLeading,
    UIStackViewAlignmentFirstBaseline,
    UIStackViewAlignmentCenter,
    UIStackViewAlignmentTrailing,
    UIStackViewAlignmentBottom = UIStackViewAlignmentTrailing,
    UIStackViewAlignmentLastBaseline,
};

@interface UIStackView : UIView {
    NSArray *_arrangedSubviews;
    UILayoutConstraintAxis _axis;
    UIStackViewDistribution _distribution;
    UIStackViewAlignment _alignment;
    CGFloat _spacing;
    BOOL _baselineRelativeArrangement;
    BOOL _layoutMarginsRelativeArrangement;
}

- (instancetype)initWithFrame:(CGRect)frame;
- (instancetype)initWithArrangedSubviews:(NSArray *)views;

@property (nonatomic, readonly, copy) NSArray *arrangedSubviews;

- (void)addArrangedSubview:(UIView *)view;
- (void)removeArrangedSubview:(UIView *)view;
- (void)insertArrangedSubview:(UIView *)view atIndex:(NSUInteger)stackIndex;

@property (nonatomic) UILayoutConstraintAxis axis;
@property (nonatomic) UIStackViewDistribution distribution;
@property (nonatomic) UIStackViewAlignment alignment;
@property (nonatomic) CGFloat spacing;

@property (nonatomic, getter=isBaselineRelativeArrangement) BOOL baselineRelativeArrangement;
@property (nonatomic, getter=isLayoutMarginsRelativeArrangement) BOOL layoutMarginsRelativeArrangement;

@end
