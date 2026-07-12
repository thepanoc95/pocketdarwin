#import <UIKit/UIStackView.h>

@implementation UIStackView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _arrangedSubviews = [[NSArray alloc] init];
        _axis = UILayoutConstraintAxisHorizontal;
        _distribution = UIStackViewDistributionFill;
        _alignment = UIStackViewAlignmentFill;
        _spacing = 0;
        _baselineRelativeArrangement = NO;
        _layoutMarginsRelativeArrangement = NO;
    }
    return self;
}

- (instancetype)initWithArrangedSubviews:(NSArray *)views {
    self = [self initWithFrame:CGRectZero];
    if (self) {
        for (UIView *view in views) {
            [self addArrangedSubview:view];
        }
    }
    return self;
}

- (void)dealloc {
    [_arrangedSubviews release];
    [super dealloc];
}

- (NSArray *)arrangedSubviews {
    return _arrangedSubviews;
}

- (void)addArrangedSubview:(UIView *)view {
    if (![_arrangedSubviews containsObject:view]) {
        _arrangedSubviews = [[_arrangedSubviews arrayByAddingObject:view] retain];
        if ([view superview] != self)
            [self addSubview:view];
        [self setNeedsLayout];
    }
}

- (void)removeArrangedSubview:(UIView *)view {
    if ([_arrangedSubviews containsObject:view]) {
        NSMutableArray *mutable = [[_arrangedSubviews mutableCopy] autorelease];
        [mutable removeObject:view];
        _arrangedSubviews = [mutable copy];
        [self setNeedsLayout];
    }
}

- (void)insertArrangedSubview:(UIView *)view atIndex:(NSUInteger)stackIndex {
    if (![_arrangedSubviews containsObject:view]) {
        NSMutableArray *mutable = [[_arrangedSubviews mutableCopy] autorelease];
        [mutable insertObject:view atIndex:stackIndex];
        _arrangedSubviews = [mutable copy];
        if ([view superview] != self)
            [self addSubview:view];
        [self setNeedsLayout];
    }
}

- (UILayoutConstraintAxis)axis {
    return _axis;
}

- (void)setAxis:(UILayoutConstraintAxis)axis {
    _axis = axis;
    [self setNeedsLayout];
}

- (UIStackViewDistribution)distribution {
    return _distribution;
}

- (void)setDistribution:(UIStackViewDistribution)distribution {
    _distribution = distribution;
    [self setNeedsLayout];
}

- (UIStackViewAlignment)alignment {
    return _alignment;
}

- (void)setAlignment:(UIStackViewAlignment)alignment {
    _alignment = alignment;
    [self setNeedsLayout];
}

- (CGFloat)spacing {
    return _spacing;
}

- (void)setSpacing:(CGFloat)spacing {
    _spacing = spacing;
    [self setNeedsLayout];
}

- (BOOL)isBaselineRelativeArrangement {
    return _baselineRelativeArrangement;
}

- (void)setBaselineRelativeArrangement:(BOOL)baseline {
    _baselineRelativeArrangement = baseline;
}

- (BOOL)isLayoutMarginsRelativeArrangement {
    return _layoutMarginsRelativeArrangement;
}

- (void)setLayoutMarginsRelativeArrangement:(BOOL)relative {
    _layoutMarginsRelativeArrangement = relative;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    CGRect bounds = [self bounds];
    NSUInteger count = [_arrangedSubviews count];
    if (count == 0)
        return;

    CGFloat totalSize = (_axis == UILayoutConstraintAxisHorizontal) ? bounds.size.width : bounds.size.height;
    CGFloat usedSize = 0;

    for (UIView *view in _arrangedSubviews) {
        CGSize viewSize = [view sizeThatFits:bounds.size];
        if (_axis == UILayoutConstraintAxisHorizontal)
            usedSize += viewSize.width;
        else
            usedSize += viewSize.height;
    }

    CGFloat spacing = (count > 1) ? _spacing * (count - 1) : 0;
    CGFloat remaining = totalSize - spacing;
    CGFloat eachSize = (count > 0 && _distribution == UIStackViewDistributionFillEqually) ? remaining / count : 0;

    CGFloat pos = 0;
    for (UIView *view in _arrangedSubviews) {
        CGSize viewSize = [view sizeThatFits:bounds.size];
        CGRect viewFrame;

        if (_axis == UILayoutConstraintAxisHorizontal) {
            CGFloat w = (_distribution == UIStackViewDistributionFillEqually) ? eachSize : viewSize.width;
            viewFrame = CGRectMake(pos, 0, w, bounds.size.height);
        } else {
            CGFloat h = (_distribution == UIStackViewDistributionFillEqually) ? eachSize : viewSize.height;
            viewFrame = CGRectMake(0, pos, bounds.size.width, h);
        }

        [view setFrame:viewFrame];
        pos += (_axis == UILayoutConstraintAxisHorizontal) ? viewFrame.size.width + _spacing : viewFrame.size.height + _spacing;
    }
}

@end
