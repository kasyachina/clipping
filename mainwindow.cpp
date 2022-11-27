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

int MainWindow::getCode(qreal x, qreal y) const
{
    int ans = 0;
    QPoint p1 = area -> getClippingWindowP1();
    QPoint p2 = area -> getClippingWindowP2();
    if (y > p1.y())
    {
        ans |= 8;
    }
    if (y < p2.y())
    {
        ans |= 4;
    }
    if (x > p2.x())
    {
        ans |= 2;
    }
    if (x < p1.x())
    {
        ans |= 1;
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
    qreal x1, x2, y1, y2;
    std::vector<std::pair<QPointF, QPointF>> data;
    for (int i = 0; i < n; ++i)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        data.push_back({QPointF(x1, y1), QPointF(x2, y2)});
    }
    fin >> x1 >> y1 >> x2 >> y2;
    area -> SetClippingWindow(QPoint(x1, y1), QPoint(x2, y2));
    fin.close();

    for (int i = 0; i < n; ++i)
    {
        x1 = data[i].first.x();
        y1 = data[i].first.y();
        x2 = data[i].second.x();
        y2 = data[i].second.y();
        int code1 = getCode(x1, y1);
        int code2 = getCode(x2, y2);
        if (code1 == 0 && code2 == 0)
        {
            //inside
            area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(x2, y2), Qt::blue});
        }
        else if ((code1 & code2) != 0)
        {
            //outside
            area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(x2, y2), Qt::red});
        }
        else
        {
            int A = y2 - y1;
            int B = x1 - x2;
            int C = -A * x1 - B * y1;
            int xmin = area -> getClippingWindowP1().x();
            int ymax = area -> getClippingWindowP1().y();
            int xmax = area -> getClippingWindowP2().x();
            int ymin = area -> getClippingWindowP2().y();
            while(code1 != 0 || code2 != 0)
            {
                if (code1 == 0)
                {
                    std::swap(x1, x2);
                    std::swap(y1, y2);
                    std::swap(code1, code2);
                }
                if (code1 & 1)
                {
                    //leftside xmin
                    qreal y = (-C - A * xmin) / (qreal)B;
                    area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(xmin, y), Qt::red});
                    x1 = xmin;
                    y1 = y;
                }
                else if (code1 & 2)
                {
                    //rightside xmax
                    qreal y = (-C - A * xmax) / (qreal)B;
                    area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(xmax, y), Qt::red});
                    x1 = xmax;
                    y1 = y;

                }
                else if (code1 & 4)
                {
                    //bottomside ymin
                    qreal x = (-C - B * ymin) / (qreal)A;
                    area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(x, ymin), Qt::red});
                    x1 = x;
                    y1 = ymin;
                }
                else
                {
                    //topside ymax
                    qreal x = (-C - B * ymax) / (qreal)A;
                    area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(x, ymax), Qt::red});
                    x1 = x;
                    y1 = ymax;
                }
                code1 = getCode(x1, y1);
            }
            area -> AddLineSegment(LineSegmentData{QPointF(x1, y1), QPointF(x2, y2), Qt::blue});
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

