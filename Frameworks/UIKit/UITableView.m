#import <UIKit/UITableView.h>
#import <UIKit/UITableViewCell.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIScreen.h>

@interface UITableViewCell (UITableViewCellInternal)
- (void)_setTableView:(UITableView *)tableView;
@end

@implementation UITableView

- (instancetype)initWithFrame:(CGRect)frame style:(UITableViewStyle)style {
    self = [super initWithFrame:frame];
    if (self) {
        _style = style;
        _dataSource = nil;
        _delegate = nil;
        _rowHeight = 44;
        _sectionHeaderHeight = 28;
        _sectionFooterHeight = 28;
        _estimatedRowHeight = 44;
        _estimatedSectionHeaderHeight = 28;
        _estimatedSectionFooterHeight = 28;
        _separatorInset = UIEdgeInsetsMake(0, 15, 0, 15);
        _separatorStyle = UITableViewCellSeparatorStyleSingleLine;
        _separatorColor = [[UIColor colorWithRed:0.78 green:0.78 blue:0.80 alpha:1] retain];
        _tableHeaderView = nil;
        _tableFooterView = nil;
        _refreshControl = nil;
        _allowsSelection = YES;
        _allowsMultipleSelection = NO;
        _allowsSelectionDuringEditing = NO;
        _allowsMultipleSelectionDuringEditing = NO;
        _editing = NO;
        _reusableCells = [[NSMutableDictionary alloc] init];
        [self setBackgroundColor:[UIColor whiteColor]];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    return [self initWithFrame:frame style:UITableViewStylePlain];
}

- (instancetype)init {
    return [self initWithFrame:CGRectZero style:UITableViewStylePlain];
}

- (UITableViewStyle)style {
    return _style;
}

- (void)dealloc {
    [_separatorColor release];
    [_tableHeaderView release];
    [_tableFooterView release];
    [_refreshControl release];
    [_reusableCells release];
    [super dealloc];
}

- (void)setDataSource:(id<UITableViewDataSource>)dataSource {
    _dataSource = dataSource;
}

- (id<UITableViewDataSource>)dataSource {
    return _dataSource;
}

- (void)setDelegate:(id<UITableViewDelegate>)delegate {
    _delegate = delegate;
}

- (id<UITableViewDelegate>)delegate {
    return _delegate;
}

- (void)setRowHeight:(CGFloat)height {
    _rowHeight = height;
}

- (CGFloat)rowHeight {
    return _rowHeight;
}

- (CGFloat)sectionHeaderHeight {
    return _sectionHeaderHeight;
}

- (void)setSectionHeaderHeight:(CGFloat)height {
    _sectionHeaderHeight = height;
}

- (CGFloat)sectionFooterHeight {
    return _sectionFooterHeight;
}

- (void)setSectionFooterHeight:(CGFloat)height {
    _sectionFooterHeight = height;
}

- (CGFloat)estimatedRowHeight {
    return _estimatedRowHeight;
}

- (void)setEstimatedRowHeight:(CGFloat)height {
    _estimatedRowHeight = height;
}

- (UIEdgeInsets)separatorInset {
    return _separatorInset;
}

- (void)setSeparatorInset:(UIEdgeInsets)inset {
    _separatorInset = inset;
}

- (UITableViewCellSeparatorStyle)separatorStyle {
    return _separatorStyle;
}

- (void)setSeparatorStyle:(UITableViewCellSeparatorStyle)style {
    _separatorStyle = style;
}

- (UIColor *)separatorColor {
    return _separatorColor;
}

- (void)setSeparatorColor:(UIColor *)color {
    color = [color retain];
    [_separatorColor release];
    _separatorColor = color;
}

- (UIView *)tableHeaderView {
    return _tableHeaderView;
}

- (void)setTableHeaderView:(UIView *)view {
    view = [view retain];
    [_tableHeaderView release];
    _tableHeaderView = view;
}

- (UIView *)tableFooterView {
    return _tableFooterView;
}

- (void)setTableFooterView:(UIView *)view {
    view = [view retain];
    [_tableFooterView release];
    _tableFooterView = view;
}

- (UIRefreshControl *)refreshControl {
    return _refreshControl;
}

- (void)setRefreshControl:(UIRefreshControl *)control {
    control = [control retain];
    [_refreshControl release];
    _refreshControl = control;
}

- (BOOL)allowsSelection {
    return _allowsSelection;
}

- (void)setAllowsSelection:(BOOL)allows {
    _allowsSelection = allows;
}

- (BOOL)allowsMultipleSelection {
    return _allowsMultipleSelection;
}

- (void)setAllowsMultipleSelection:(BOOL)allows {
    _allowsMultipleSelection = allows;
}

- (BOOL)isEditing {
    return _editing;
}

- (void)setEditing:(BOOL)editing {
    _editing = editing;
}

- (void)reloadData {
    [self setNeedsLayout];
}

- (void)reloadRowsAtIndexPaths:(NSArray *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation {
}

- (void)reloadSections:(NSIndexSet *)sections withRowAnimation:(UITableViewRowAnimation)animation {
}

- (NSInteger)numberOfSections {
    if ([_dataSource respondsToSelector:@selector(numberOfSectionsInTableView:)])
        return [_dataSource numberOfSectionsInTableView:self];
    return 1;
}

- (NSInteger)numberOfRowsInSection:(NSInteger)section {
    return [_dataSource tableView:self numberOfRowsInSection:section];
}

- (UITableViewCell *)cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    return nil;
}

- (NSIndexPath *)indexPathForCell:(UITableViewCell *)cell {
    return nil;
}

- (NSIndexPath *)indexPathForRowAtPoint:(CGPoint)point {
    return nil;
}

- (NSArray *)visibleCells {
    return @[];
}

- (NSArray *)indexPathsForVisibleRows {
    return @[];
}

- (void)scrollToRowAtIndexPath:(NSIndexPath *)indexPath atScrollPosition:(UITableViewScrollPosition)scrollPosition animated:(BOOL)animated {
}

- (void)scrollToNearestSelectedRowAtScrollPosition:(UITableViewScrollPosition)scrollPosition animated:(BOOL)animated {
}

- (void)insertSections:(NSIndexSet *)sections withRowAnimation:(UITableViewRowAnimation)animation {
}

- (void)deleteSections:(NSIndexSet *)sections withRowAnimation:(UITableViewRowAnimation)animation {
}

- (void)insertRowsAtIndexPaths:(NSArray *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation {
}

- (void)deleteRowsAtIndexPaths:(NSArray *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation {
}

- (void)moveSection:(NSInteger)section toSection:(NSInteger)newSection {
}

- (void)moveRowAtIndexPath:(NSIndexPath *)indexPath toIndexPath:(NSIndexPath *)newIndexPath {
}

- (void)beginUpdates {
}

- (void)endUpdates {
}

- (UITableViewCell *)dequeueReusableCellWithIdentifier:(NSString *)identifier {
    if (!identifier)
        return nil;
    NSMutableSet *cells = [_reusableCells objectForKey:identifier];
    UITableViewCell *cell = [cells anyObject];
    if (cell) {
        [cell retain];
        [cells removeObject:cell];
        [cell prepareForReuse];
        return [cell autorelease];
    }
    return nil;
}

- (UITableViewCell *)dequeueReusableCellWithIdentifier:(NSString *)identifier forIndexPath:(NSIndexPath *)indexPath {
    return [self dequeueReusableCellWithIdentifier:identifier];
}

- (void)registerClass:(Class)cellClass forCellReuseIdentifier:(NSString *)identifier {
}

- (void)registerNib:(UINib *)nib forCellReuseIdentifier:(NSString *)identifier {
}

- (NSIndexPath *)indexPathForSelectedRow {
    return nil;
}

- (NSArray *)indexPathsForSelectedRows {
    return nil;
}

- (void)selectRowAtIndexPath:(NSIndexPath *)indexPath animated:(BOOL)animated scrollPosition:(UITableViewScrollPosition)scrollPosition {
}

- (void)deselectRowAtIndexPath:(NSIndexPath *)indexPath animated:(BOOL)animated {
}

- (void)setEditing:(BOOL)editing animated:(BOOL)animated {
    _editing = editing;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    if (!_dataSource)
        return;

    CGRect bounds = [self bounds];
    CGFloat y = 0;
    NSInteger sections = [self numberOfSections];

    for (NSInteger section = 0; section < sections; section++) {
        NSInteger rows = [self numberOfRowsInSection:section];
        for (NSInteger row = 0; row < rows; row++) {
            NSIndexPath *indexPath = [NSIndexPath indexPathForRow:row inSection:section];
            UITableViewCell *cell = [_dataSource tableView:self cellForRowAtIndexPath:indexPath];
            if (cell) {
                [cell _setTableView:self];
                CGRect cellFrame = CGRectMake(0, y, bounds.size.width, _rowHeight);
                [cell setFrame:cellFrame];
                if ([cell superview] != self)
                    [self addSubview:cell];
                y += _rowHeight;
            }
        }
    }
}

- (CGSize)sizeThatFits:(CGSize)size {
    if (!_dataSource)
        return size;
    CGFloat height = 0;
    NSInteger sections = [self numberOfSections];
    for (NSInteger section = 0; section < sections; section++) {
        height += _sectionHeaderHeight;
        height += [self numberOfRowsInSection:section] * _rowHeight;
        height += _sectionFooterHeight;
    }
    return CGSizeMake(size.width, height);
}

@end
