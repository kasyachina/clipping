#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <fstream>
#include <QMessageBox>

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

int MainWindow::getCode(const QPoint& p) const
{
    int ans = 0;
    int x = p.x();
    int y = p.y();
    QPoint p1 = area -> getClippingWindowP1();
    QPoint p2 = area -> getClippingWindowP2();
    if (y > p1.y())
    {
        ans |= 1;
    }
    if (y < p2.y())
    {
        ans |= 2;
    }
    if (x > p2.x())
    {
        ans |= 4;
    }
    if (x < p1.x())
    {
        ans |= 8;
    }
    return ans;
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReadSegments()
{
    std::ifstream fin(segmentsPath.toStdString());
    if (!fin)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Файл segments.txt в папке исполняемого файла не найден");
        return;
    }
    int n;
    fin >> n;
    int x1, x2, y1, y2;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        area->AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::blue});
    }
    fin >> x1 >> y1 >> x2 >> y2;
    area -> SetClippingWindow({x1, y1}, {x2, y2});
    fin.close();
}
void MainWindow::ReadPoly()
{
    std::ifstream fin(polygonPath.toStdString());
    if (!fin)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Файл polygon.txt в папке исполняемого файла не найден");
        return;
    }
    int n;
    fin >> n;
    int x1, y1;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1;
        area -> AddPolygonPoint(x1, y1);
    }
    int x2, y2;
    fin >> x1 >> y1 >> x2 >> y2;
    area -> SetClippingWindow({x1, y1}, {x2, y2});
    fin.close();
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

