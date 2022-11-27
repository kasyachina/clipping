#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <fstream>
#include <QMessageBox>
#include <QDebug>

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

int MainWindow::getCode(int x, int y) const
{
    int ans = 0;
    QPoint p1 = area -> getClippingWindowP1();
    QPoint p2 = area -> getClippingWindowP2();
    if (y >= p1.y())
    {
        ans |= 1;
    }
    if (y <= p2.y())
    {
        ans |= 2;
    }
    if (x >= p2.x())
    {
        ans |= 4;
    }
    if (x <= p1.x())
    {
        ans |= 8;
    }
    return ans;
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ProcessSegments()
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
    std::vector<std::pair<QPoint, QPoint>> data;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        data.push_back({QPoint(x1, y1), QPoint(x2, y2)});
    }
    fin >> x1 >> y1 >> x2 >> y2;
    area -> SetClippingWindow({x1, y1}, {x2, y2});
    fin.close();

    for (int i = 0; i < n; ++i)
    {
        int x1 = data[i].first.x();
        int y1 = data[i].first.y();
        int x2 = data[i].second.x();
        int y2 = data[i].second.y();
        int code1 = getCode(x1, y1);
        int code2 = getCode(x2, y2);
        if (code1 == 0 && code2 == 0)
        {
            //inside
            area -> AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::blue});
        }
        else if ((code1 & code2) != 0)
        {
            //outside
            area -> AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::red});
        }
        else if ((code1 == 0 && code2 != 0) || (code2 == 0 && code1 != 0))
        {
            area -> AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::magenta});
        }
        else
        {
            area -> AddLineSegment(LineSegmentData{QPoint(x1, y1), QPoint(x2, y2), Qt::magenta});
        }
    }
}
void MainWindow::ProcessPoly()
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
    ProcessSegments();
    area -> repaint();
}


void MainWindow::on_poly_clicked()
{
    area -> Clear();
    area -> ChangeMode(PlotMode::Polygons);
    ProcessPoly();
    area -> repaint();
}

