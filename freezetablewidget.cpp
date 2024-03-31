#include "freezetablewidget.h"

#include<QScrollBar>
#include<QHeaderView>
#include<QDebug>

FreezeTableWidget::FreezeTableWidget(QAbstractItemModel *model, FreezeType type)
{
    setModel(model);
    fType = type;
    switch (fType) {
    case First:
        initFirst();
        break;
    case Last:
        initLast();
        break;
    case Both:{
        initFirst();
        initLast();
    }
    break;
    default:
        break;
    }
}

FreezeTableWidget::~FreezeTableWidget()
{
    if (frozenLastTableView) {
        delete frozenLastTableView;
    }
    if (frozenTableView) {
        delete frozenTableView;
    }
}

void FreezeTableWidget::initFirst()
{
    frozenTableView = new QTableView(this);
    connect(horizontalHeader(), &QHeaderView::sectionResized, this, &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized, this, &FreezeTableWidget::updateSectionHeight);
    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged, frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);

    frozenTableView->setModel(model());
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->verticalHeader()->hide();
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    viewport()->stackUnder(frozenTableView);

    frozenTableView->setStyleSheet("QTableView { border: none;"
                                   "background-color: #8EDE21;"
                                   "selection-background-color: #999}"); //for demo purposes
    frozenTableView->setSelectionModel(selectionModel());

    for (int col = 1; col < model()->columnCount(); ++col)
        frozenTableView->setColumnHidden(col, true);
    frozenTableView->setColumnWidth(0, columnWidth(0));

    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->show();

    updateFrozenTableGeometry();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenTableView->setVerticalScrollMode(ScrollPerPixel);
}

void FreezeTableWidget::initLast()
{
    frozenLastTableView = new QTableView(this);
    connect(horizontalHeader(), &QHeaderView::sectionResized, this, &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized, this, &FreezeTableWidget::updateSectionHeight);
    connect(frozenLastTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged, frozenLastTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    frozenLastTableView->setModel(model());
    frozenLastTableView->setFocusPolicy(Qt::NoFocus);
    frozenLastTableView->verticalHeader()->hide();
    frozenLastTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    if (fType == Last) {
        viewport()->stackUnder(frozenLastTableView);
    }


    frozenLastTableView->setStyleSheet("QTableView { border: none;"
                                   "background-color: #8EDE21;"
                                   "selection-background-color: #999}"); //for demo purposes
    frozenLastTableView->setSelectionModel(selectionModel());

    for (int col = 0; col < model()->columnCount()-1; ++col)
        frozenLastTableView->setColumnHidden(col, true);
    frozenLastTableView->setColumnWidth(model()->columnCount()-1, columnWidth(model()->columnCount()-1) );

    frozenLastTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenLastTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenLastTableView->show();

    updateFrozenTableGeometry();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenLastTableView->setVerticalScrollMode(ScrollPerPixel);
}

void FreezeTableWidget::updateFrozenTableGeometry()
{
    if (frozenTableView != nullptr) {
        qDebug()<<"first";
        frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                     frameWidth(), columnWidth(0),
                                     viewport()->height()+horizontalHeader()->height());
    }
    if (frozenLastTableView != nullptr) {
        frozenLastTableView->setGeometry(viewport()->width() - frameWidth() - columnWidth(model()->columnCount()-1) + verticalHeader()->width(),frameWidth(),
                                     columnWidth(model()->columnCount()-1), viewport()->height()+horizontalHeader()->height());
    }
}

void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /*oldSize*/, int newSize)
{
    if (fType == First) {
        if (logicalIndex == 0){
            frozenTableView->setColumnWidth(0, newSize);
        }
    } else if (fType == Last) {
        if (logicalIndex == model()->columnCount()-1){
            frozenTableView->setColumnWidth(model()->columnCount()-1, newSize);
        }
    } else if (fType == Both) {
        if (logicalIndex == 0){
            frozenTableView->setColumnWidth(0, newSize);
        }
        if (logicalIndex == model()->columnCount()-1){
            frozenLastTableView->setColumnWidth(model()->columnCount()-1, newSize);

        }
    }
    updateFrozenTableGeometry();
}

void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /*oldSize*/, int newSize)
{
    frozenTableView->setRowHeight(logicalIndex, newSize);
}

void FreezeTableWidget::focusOutEvent(QFocusEvent *event)
{
    if (!selectedIndexes().isEmpty()) {
        clearSelection();
    }
    QTableView::focusOutEvent(event);
}

void FreezeTableWidget::resizeEvent(QResizeEvent * event)
{
    QTableView::resizeEvent(event);
    if (fType != None) {
        updateFrozenTableGeometry();
    }

}

QModelIndex FreezeTableWidget::moveCursor(CursorAction cursorAction,
                                          Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (frozenTableView && cursorAction == MoveLeft && current.column() > 0
        && visualRect(current).topLeft().x() < frozenTableView->columnWidth(0)) {
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                             - frozenTableView->columnWidth(0);
        horizontalScrollBar()->setValue(newValue);
    }

    if (frozenLastTableView && cursorAction == MoveRight && current.column() < model()->columnCount()
        && visualRect(current).topRight().x() > viewport()->width() - frozenLastTableView->columnWidth(model()->columnCount()-1)) {
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topRight().x() - viewport()->width()
                             + frozenLastTableView->columnWidth(model()->columnCount()-1);
        horizontalScrollBar()->setValue(newValue);
    }

    return current;
}

void FreezeTableWidget::scrollTo (const QModelIndex & index, ScrollHint hint){
    if (index.column() > 0)
        QTableView::scrollTo(index, hint);
}
