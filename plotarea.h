#ifndef PLOTAREA_H
#define PLOTAREA_H

#include <QPainter>
#include <QWidget>
#include <vector>

class LineSegmentData
{
public:
    explicit LineSegmentData(const QPoint& p1, const QPoint& p2, const QColor& color);
    int x1() const;
    int x2() const;
    int y1() const;
    int y2() const;
    QPoint p1() const;
    QPoint p2() const;
    QColor color() const;
private:
    QPoint _p1, _p2;
    QColor _color;
};

enum class PlotMode
{
    None,
    Segments,
    Polygons,
};

class PlotArea : public QWidget
{
    Q_OBJECT
public:
    explicit PlotArea(QWidget *parent = nullptr, PlotMode mode = PlotMode::None);
    void AddLineSegment(const LineSegmentData& data);
    QPoint Adjust(const QPoint& p);
    void AddPolygonPoint(int x, int y);
    void SetPolygonFillingColor(const QColor& color);
    void SetPolygonBorderColor(const QColor& color);
    void SetClippingWindow(const QPoint& p1, const QPoint& p2);
    void ChangeMode(PlotMode newMode);
    void Clear();
    void SetUnit(int nu);
    int getUnit() const;
private:
    int u; // unit size
    int tick_length = 4;
    int grid_line_width = 1;
    int axis_width = 2;
    int box_offset = 1;
    int box_width = 1;
    int pixel_width = 1;
    int line_width = 2;
    int zx = 0;
    int zy = 0;
    PlotMode mode = PlotMode::None;
    std::vector<LineSegmentData> segments;
    std::vector<QPoint> polygonData;
    QColor polygonFillingColor = Qt::white;
    QColor polygonBorderColor = Qt::black;
    QColor clippingWindowColor = Qt::red;
    QColor axisColor = Qt::black;
    QColor gridColor = Qt::gray;
    QColor boxColor = Qt::gray;
    QPoint clippingWindowp1;
    QPoint clippingWindowp2;
    void inline drawBox(QPainter(&p));
    void inline drawGrid(QPainter& p);
    void inline drawAxis(QPainter& p);
    void inline drawTicks(QPainter& p);
    void inline drawArrows(QPainter& p);
    void inline drawLineSegments(QPainter& p);
    void inline drawPolygon(QPainter& p);
    void inline drawClippingWindow(QPainter& p);
    void paintEvent(QPaintEvent* event) override;
};

#endif // PLOTAREA_H
