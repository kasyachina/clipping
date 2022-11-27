#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("background-color: white;");
    QGridLayout *g = new QGridLayout(centralWidget());
    area = new PlotArea();
    g -> addWidget(area, 0, 0, 4, 4);
    centralWidget()->setLayout(g);
    QString filepath = "/home/kasinski_nikita/Documents/input.txt";
    std::ifstream fin(filepath.toStdString());
    int n;
    fin >> n;
    int x1, x2, y1, y2;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        area->AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::blue});
    }
    area->repaint();
    setMinimumSize({600, 600});
    setWindowTitle("Отсечения");
}

MainWindow::~MainWindow()
{
    delete ui;
}

