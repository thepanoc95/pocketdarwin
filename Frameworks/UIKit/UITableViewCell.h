#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>

@class UILabel;
@class UIImageView;
@class UITableView;

typedef NSInteger UITableViewCellStyle;

enum {
    UITableViewCellStyleDefault,
    UITableViewCellStyleValue1,
    UITableViewCellStyleValue2,
    UITableViewCellStyleSubtitle,
};

typedef NSInteger UITableViewCellStateMask;

enum {
    UITableViewCellStateDefaultMask                     = 0,
    UITableViewCellStateShowingEditControlMask           = 1 << 0,
    UITableViewCellStateShowingDeleteConfirmationMask    = 1 << 1,
};

@interface UITableViewCell : UIView {
    UITableViewCellStyle _style;
    NSString *_reuseIdentifier;
    UILabel *_textLabel;
    UILabel *_detailTextLabel;
    UIImageView *_imageView;
    UIView *_contentView;
    UIView *_backgroundView;
    UIView *_selectedBackgroundView;
    UIView *_accessoryView;
    UIView *_editingAccessoryView;
    UITableViewCellAccessoryType _accessoryType;
    UITableViewCellAccessoryType _editingAccessoryType;
    UITableViewCellSelectionStyle _selectionStyle;
    UITableViewCellSeparatorStyle _separatorStyle;
    NSInteger _indentationLevel;
    CGFloat _indentationWidth;
    BOOL _selected;
    BOOL _highlighted;
    BOOL _showsReorderControl;
    BOOL _shouldIndentWhileEditing;
    BOOL _editing;
    UITableView *_tableView;
}

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic, readonly, copy) NSString *reuseIdentifier;

- (void)prepareForReuse;

@property (nonatomic, readonly, retain) UILabel *textLabel;
@property (nonatomic, readonly, retain) UILabel *detailTextLabel;
@property (nonatomic, readonly, retain) UIImageView *imageView;
@property (nonatomic, readonly, retain) UIView *contentView;

@property (nonatomic, retain) UIView *backgroundView;
@property (nonatomic, retain) UIView *selectedBackgroundView;

@property (nonatomic, retain) UIView *accessoryView;
@property (nonatomic) UITableViewCellAccessoryType accessoryType;

@property (nonatomic, retain) UIView *editingAccessoryView;
@property (nonatomic) UITableViewCellAccessoryType editingAccessoryType;

@property (nonatomic) UITableViewCellSelectionStyle selectionStyle;

@property (nonatomic, getter=isSelected) BOOL selected;
@property (nonatomic, getter=isHighlighted) BOOL highlighted;

@property (nonatomic) NSInteger indentationLevel;
@property (nonatomic) CGFloat indentationWidth;

@property (nonatomic) BOOL showsReorderControl;
@property (nonatomic) BOOL shouldIndentWhileEditing;
@property (nonatomic, getter=isEditing) BOOL editing;

- (void)willTransitionToState:(UITableViewCellStateMask)state;
- (void)didTransitionToState:(UITableViewCellStateMask)state;

- (void)setSelected:(BOOL)selected animated:(BOOL)animated;
- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated;

- (CGSize)sizeThatFits:(CGSize)size;

- (void)_setTableView:(UITableView *)tableView;

@end
