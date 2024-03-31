#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "freezetablewidget.h"
#include<QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *newItem = nullptr;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 10; j++) {
            newItem = new QStandardItem(QString("%1-%2").arg(i).arg(j));
            model->setItem(i, j, newItem);
        }
    }
    FreezeTableWidget *table = new FreezeTableWidget(model, FreezeTableWidget::Both);
    table->resize(560, 680);
    table->show();
    setCentralWidget(table);
}

MainWindow::~MainWindow()
{
    delete ui;
}
