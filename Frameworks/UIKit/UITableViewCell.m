#import <UIKit/UITableViewCell.h>
#import <UIKit/UILabel.h>
#import <UIKit/UIImageView.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIFont.h>

@implementation UITableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithFrame:CGRectMake(0, 0, 320, 44)];
    if (self) {
        _style = style;
        _reuseIdentifier = [reuseIdentifier copy];
        _selectionStyle = UITableViewCellSelectionStyleDefault;
        _accessoryType = UITableViewCellAccessoryNone;
        _editingAccessoryType = UITableViewCellAccessoryNone;
        _indentationLevel = 0;
        _indentationWidth = 10;
        _separatorStyle = UITableViewCellSeparatorStyleSingleLine;
        _selected = NO;
        _highlighted = NO;
        _showsReorderControl = NO;
        _shouldIndentWhileEditing = YES;
        _editing = NO;
        _tableView = nil;

        _contentView = [[UIView alloc] initWithFrame:[self bounds]];
        [_contentView setBackgroundColor:[UIColor clearColor]];
        [self addSubview:_contentView];

        _textLabel = [[UILabel alloc] initWithFrame:CGRectZero];
        [_textLabel setBackgroundColor:[UIColor clearColor]];
        [_textLabel setFont:[UIFont systemFontOfSize:17]];
        [_contentView addSubview:_textLabel];

        _detailTextLabel = [[UILabel alloc] initWithFrame:CGRectZero];
        [_detailTextLabel setBackgroundColor:[UIColor clearColor]];
        [_detailTextLabel setFont:[UIFont systemFontOfSize:14]];
        [_detailTextLabel setTextColor:[UIColor grayColor]];
        [_contentView addSubview:_detailTextLabel];

        _imageView = [[UIImageView alloc] initWithFrame:CGRectZero];
        [_contentView addSubview:_imageView];

        [self setBackgroundColor:[UIColor whiteColor]];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    return [self initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];
}

- (void)dealloc {
    [_reuseIdentifier release];
    [_textLabel release];
    [_detailTextLabel release];
    [_imageView release];
    [_contentView release];
    [_backgroundView release];
    [_selectedBackgroundView release];
    [_accessoryView release];
    [_editingAccessoryView release];
    [super dealloc];
}

- (NSString *)reuseIdentifier {
    return _reuseIdentifier;
}

- (void)prepareForReuse {
}

- (UILabel *)textLabel {
    return _textLabel;
}

- (UILabel *)detailTextLabel {
    return _detailTextLabel;
}

- (UIImageView *)imageView {
    return _imageView;
}

- (UIView *)contentView {
    return _contentView;
}

- (UIView *)backgroundView {
    return _backgroundView;
}

- (void)setBackgroundView:(UIView *)view {
    view = [view retain];
    [_backgroundView removeFromSuperview];
    [_backgroundView release];
    _backgroundView = view;
    if (view) {
        [self insertSubview:view atIndex:0];
    }
}

- (UIView *)selectedBackgroundView {
    return _selectedBackgroundView;
}

- (void)setSelectedBackgroundView:(UIView *)view {
    view = [view retain];
    [_selectedBackgroundView removeFromSuperview];
    [_selectedBackgroundView release];
    _selectedBackgroundView = view;
}

- (UIView *)accessoryView {
    return _accessoryView;
}

- (void)setAccessoryView:(UIView *)view {
    view = [view retain];
    [_accessoryView release];
    _accessoryView = view;
}

- (UITableViewCellAccessoryType)accessoryType {
    return _accessoryType;
}

- (void)setAccessoryType:(UITableViewCellAccessoryType)type {
    _accessoryType = type;
}

- (UIView *)editingAccessoryView {
    return _editingAccessoryView;
}

- (void)setEditingAccessoryView:(UIView *)view {
    view = [view retain];
    [_editingAccessoryView release];
    _editingAccessoryView = view;
}

- (UITableViewCellAccessoryType)editingAccessoryType {
    return _editingAccessoryType;
}

- (void)setEditingAccessoryType:(UITableViewCellAccessoryType)type {
    _editingAccessoryType = type;
}

- (UITableViewCellSelectionStyle)selectionStyle {
    return _selectionStyle;
}

- (void)setSelectionStyle:(UITableViewCellSelectionStyle)style {
    _selectionStyle = style;
}

- (BOOL)isSelected {
    return _selected;
}

- (void)setSelected:(BOOL)selected {
    _selected = selected;
}

- (BOOL)isHighlighted {
    return _highlighted;
}

- (void)setHighlighted:(BOOL)highlighted {
    _highlighted = highlighted;
}

- (NSInteger)indentationLevel {
    return _indentationLevel;
}

- (void)setIndentationLevel:(NSInteger)level {
    _indentationLevel = level;
}

- (CGFloat)indentationWidth {
    return _indentationWidth;
}

- (void)setIndentationWidth:(CGFloat)width {
    _indentationWidth = width;
}

- (BOOL)showsReorderControl {
    return _showsReorderControl;
}

- (void)setShowsReorderControl:(BOOL)shows {
    _showsReorderControl = shows;
}

- (BOOL)shouldIndentWhileEditing {
    return _shouldIndentWhileEditing;
}

- (void)setShouldIndentWhileEditing:(BOOL)shouldIndent {
    _shouldIndentWhileEditing = shouldIndent;
}

- (BOOL)isEditing {
    return _editing;
}

- (void)setEditing:(BOOL)editing {
    _editing = editing;
}

- (void)willTransitionToState:(UITableViewCellStateMask)state {
}

- (void)didTransitionToState:(UITableViewCellStateMask)state {
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    _selected = selected;
}

- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated {
    _highlighted = highlighted;
}

- (void)_setTableView:(UITableView *)tableView {
    _tableView = tableView;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    CGRect bounds = [self bounds];
    [_contentView setFrame:bounds];

    CGFloat x = _indentationLevel * _indentationWidth + 15;
    CGFloat width = bounds.size.width - x - 15;
    CGFloat height = bounds.size.height;

    if (_imageView && [_imageView image]) {
        [_imageView setFrame:CGRectMake(x, (height - 40) / 2, 40, 40)];
        x += 50;
        width -= 50;
    } else {
        [_imageView setFrame:CGRectZero];
    }

    if (_style == UITableViewCellStyleSubtitle) {
        [_textLabel setFrame:CGRectMake(x, (height - 40) / 2 - 5, width, 22)];
        [_detailTextLabel setFrame:CGRectMake(x, (height - 40) / 2 + 17, width, 18)];
    } else if (_style == UITableViewCellStyleValue1) {
        [_textLabel setFrame:CGRectMake(x, 0, width * 0.6, height)];
        [_detailTextLabel setFrame:CGRectMake(width * 0.6 + 10, 0, width * 0.4 - 10, height)];
        [_detailTextLabel setTextAlignment:NSTextAlignmentRight];
    } else {
        [_textLabel setFrame:CGRectMake(x, 0, width, height)];
        [_detailTextLabel setFrame:CGRectZero];
    }
}

- (CGSize)sizeThatFits:(CGSize)size {
    return CGSizeMake(size.width, 44);
}

@end
