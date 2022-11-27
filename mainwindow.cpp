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
    g -> addWidget(area, 0, 0, 10, 4);
    g -> addWidget(ui -> clippingText,  0, 4, 1, 1);
    g -> addWidget(ui -> segments,      1, 4, 1, 1);
    g -> addWidget(ui -> poly,          2, 4, 1, 1);
    centralWidget()->setLayout(g);
    setMinimumSize({600, 600});
    setWindowTitle("Отсечения");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReadSegments()
{
    std::ifstream fin(segmentsPath.toStdString());
    int n;
    fin >> n;
    int x1, x2, y1, y2;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        area->AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::blue});
    }
}
void MainWindow::ReadPoly()
{
    std::ifstream fin(polygonPath.toStdString());
    int n;
    fin >> n;
    int x1, y1;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1;
        area ->
    }
}
void MainWindow::on_segments_clicked()
{
    area -> Clear();
    area -> ChangeMode(PlotMode::Segments);
    ReadSegments();
    area -> repaint();
}


void MainWindow::on_poly_clicked()
{
    area -> Clear();
    area -> ChangeMode(PlotMode::Polygons);
    ReadPoly();
    area -> repaint();
}

