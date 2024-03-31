#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H

#include<QTableView>

class FreezeTableWidget : public QTableView
{
    Q_OBJECT
public:
    enum FreezeType {
        None,
        First,
        Last,
        Both
    };
public:
    FreezeTableWidget(QAbstractItemModel *model = nullptr, enum FreezeType type = FreezeType::None);
    ~FreezeTableWidget();

protected:
    void resizeEvent(QResizeEvent *event) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;

private:
    QTableView *frozenTableView = nullptr;
    QTableView *frozenLastTableView = nullptr;
    enum FreezeType fType = None;
    void initFirst();
    void initLast();
    void updateFrozenTableGeometry();

private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);


    // QWidget interface
protected:
    void focusOutEvent(QFocusEvent *event) override;
};

#endif // FREEZETABLEWIDGET_H
