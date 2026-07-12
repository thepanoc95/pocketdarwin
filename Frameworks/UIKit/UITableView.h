#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>

@class UITableViewCell;
@class UITableViewHeaderFooterView;
@class NSIndexPath;
@class UIRefreshControl;
@class UIDataSourceModelAssociation;

typedef NSInteger UITableViewStyle;

enum {
    UITableViewStylePlain,
    UITableViewStyleGrouped,
    UITableViewStyleInsetGrouped,
};

typedef NSInteger UITableViewScrollPosition;

enum {
    UITableViewScrollPositionNone,
    UITableViewScrollPositionTop,
    UITableViewScrollPositionMiddle,
    UITableViewScrollPositionBottom,
};

typedef NSInteger UITableViewRowAnimation;

enum {
    UITableViewRowAnimationFade,
    UITableViewRowAnimationRight,
    UITableViewRowAnimationLeft,
    UITableViewRowAnimationTop,
    UITableViewRowAnimationBottom,
    UITableViewRowAnimationNone,
    UITableViewRowAnimationMiddle,
    UITableViewRowAnimationAutomatic = 100,
};

typedef NSInteger UITableViewCellSeparatorStyle;

enum {
    UITableViewCellSeparatorStyleNone,
    UITableViewCellSeparatorStyleSingleLine,
    UITableViewCellSeparatorStyleSingleLineEtched,
};

typedef NSInteger UITableViewCellSelectionStyle;

enum {
    UITableViewCellSelectionStyleNone,
    UITableViewCellSelectionStyleBlue,
    UITableViewCellSelectionStyleGray,
    UITableViewCellSelectionStyleDefault,
};

typedef NSInteger UITableViewCellAccessoryType;

enum {
    UITableViewCellAccessoryNone,
    UITableViewCellAccessoryDisclosureIndicator,
    UITableViewCellAccessoryDetailDisclosureButton,
    UITableViewCellAccessoryCheckmark,
    UITableViewCellAccessoryDetailButton,
};

typedef NSInteger UITableViewCellEditingStyle;

enum {
    UITableViewCellEditingStyleNone,
    UITableViewCellEditingStyleDelete,
    UITableViewCellEditingStyleInsert,
};

@protocol UITableViewDataSource <NSObject>

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

@optional
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView;
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section;
- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section;

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath;
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath;

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath;
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)sourceIndexPath toIndexPath:(NSIndexPath *)destinationIndexPath;

@end

@protocol UITableViewDelegate <NSObject>
@optional

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath;
- (void)tableView:(UITableView *)tableView didDeselectRowAtIndexPath:(NSIndexPath *)indexPath;

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath;
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section;
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section;

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section;
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section;

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath;

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath;

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath;
- (NSIndexPath *)tableView:(UITableView *)tableView willDeselectRowAtIndexPath:(NSIndexPath *)indexPath;

@end

@interface UITableView : UIView <NSCoding> {
    UITableViewStyle _style;
    id<UITableViewDataSource> _dataSource;
    id<UITableViewDelegate> _delegate;
    CGFloat _rowHeight;
    CGFloat _sectionHeaderHeight;
    CGFloat _sectionFooterHeight;
    CGFloat _estimatedRowHeight;
    CGFloat _estimatedSectionHeaderHeight;
    CGFloat _estimatedSectionFooterHeight;
    UIEdgeInsets _separatorInset;
    UITableViewCellSeparatorStyle _separatorStyle;
    UIColor *_separatorColor;
    UIView *_tableHeaderView;
    UIView *_tableFooterView;
    UIRefreshControl *_refreshControl;
    BOOL _allowsSelection;
    BOOL _allowsMultipleSelection;
    BOOL _allowsSelectionDuringEditing;
    BOOL _allowsMultipleSelectionDuringEditing;
    BOOL _editing;
    NSMutableDictionary *_reusableCells;
}

- (instancetype)initWithFrame:(CGRect)frame style:(UITableViewStyle)style;

@property (nonatomic, assign) id<UITableViewDataSource> dataSource;
@property (nonatomic, assign) id<UITableViewDelegate> delegate;

@property (nonatomic) CGFloat rowHeight;
@property (nonatomic) CGFloat sectionHeaderHeight;
@property (nonatomic) CGFloat sectionFooterHeight;

@property (nonatomic) CGFloat estimatedRowHeight;
@property (nonatomic) CGFloat estimatedSectionHeaderHeight;
@property (nonatomic) CGFloat estimatedSectionFooterHeight;

@property (nonatomic) UIEdgeInsets separatorInset;
@property (nonatomic) UITableViewCellSeparatorStyle separatorStyle;
@property (nonatomic, retain) UIColor *separatorColor;

@property (nonatomic, retain) UIView *tableHeaderView;
@property (nonatomic, retain) UIView *tableFooterView;

@property (nonatomic, retain) UIRefreshControl *refreshControl;

@property (nonatomic) BOOL allowsSelection;
@property (nonatomic) BOOL allowsMultipleSelection;
@property (nonatomic) BOOL allowsSelectionDuringEditing;
@property (nonatomic) BOOL allowsMultipleSelectionDuringEditing;

@property (nonatomic, getter=isEditing) BOOL editing;

- (void)reloadData;
- (void)reloadRowsAtIndexPaths:(NSArray *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation;
- (void)reloadSections:(NSIndexSet *)sections withRowAnimation:(UITableViewRowAnimation)animation;

- (NSInteger)numberOfSections;
- (NSInteger)numberOfRowsInSection:(NSInteger)section;

- (UITableViewCell *)cellForRowAtIndexPath:(NSIndexPath *)indexPath;
- (NSIndexPath *)indexPathForCell:(UITableViewCell *)cell;
- (NSIndexPath *)indexPathForRowAtPoint:(CGPoint)point;

- (NSArray *)visibleCells;
- (NSArray *)indexPathsForVisibleRows;

- (void)scrollToRowAtIndexPath:(NSIndexPath *)indexPath atScrollPosition:(UITableViewScrollPosition)scrollPosition animated:(BOOL)animated;
- (void)scrollToNearestSelectedRowAtScrollPosition:(UITableViewScrollPosition)scrollPosition animated:(BOOL)animated;

- (void)insertSections:(NSIndexSet *)sections withRowAnimation:(UITableViewRowAnimation)animation;
- (void)deleteSections:(NSIndexSet *)sections withRowAnimation:(UITableViewRowAnimation)animation;

- (void)insertRowsAtIndexPaths:(NSArray *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation;
- (void)deleteRowsAtIndexPaths:(NSArray *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation;

- (void)moveSection:(NSInteger)section toSection:(NSInteger)newSection;
- (void)moveRowAtIndexPath:(NSIndexPath *)indexPath toIndexPath:(NSIndexPath *)newIndexPath;

- (void)beginUpdates;
- (void)endUpdates;

- (UITableViewCell *)dequeueReusableCellWithIdentifier:(NSString *)identifier;
- (UITableViewCell *)dequeueReusableCellWithIdentifier:(NSString *)identifier forIndexPath:(NSIndexPath *)indexPath;

- (void)registerClass:(Class)cellClass forCellReuseIdentifier:(NSString *)identifier;
- (void)registerNib:(UINib *)nib forCellReuseIdentifier:(NSString *)identifier;

- (NSIndexPath *)indexPathForSelectedRow;
- (NSArray *)indexPathsForSelectedRows;

- (void)selectRowAtIndexPath:(NSIndexPath *)indexPath animated:(BOOL)animated scrollPosition:(UITableViewScrollPosition)scrollPosition;
- (void)deselectRowAtIndexPath:(NSIndexPath *)indexPath animated:(BOOL)animated;

- (void)setEditing:(BOOL)editing animated:(BOOL)animated;

@end
