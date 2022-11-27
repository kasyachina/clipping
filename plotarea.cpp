#include "plotarea.h"
#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>

LineSegmentData::LineSegmentData(const QPoint& p1, const QPoint& p2, const QColor& color)
{
    _p1 = p1;
    _p2 = p2;
    _color = color;
}

int LineSegmentData::x1() const
{
    return _p1.x();
}

int LineSegmentData::x2() const
{
    return _p2.x();
}

int LineSegmentData::y1() const
{
    return _p1.y();
}

int LineSegmentData::y2() const
{
    return _p2.y();
}
QPoint LineSegmentData::p1() const
{
    return _p1;
}
QPoint LineSegmentData::p2() const
{
    return _p2;
}
QColor LineSegmentData::color() const
{
    return _color;
}
PlotArea::PlotArea(QWidget *parent, PlotMode _mode):QWidget(parent)
{
    u = std::min(width(), height()) / 20;
    mode = _mode;
}
QPoint PlotArea::Adjust(const QPoint& p)
{
    return QPoint(zx + p.x() * u, zy - p.y() * u);
}
void PlotArea::drawBox(QPainter& p)
{
    int h = height() - 2 * box_offset;
    int w = width() - 2 * box_offset;
    QPen boxPen(boxColor);
    boxPen.setWidth(box_width);
    p.setPen(boxPen);
    p.drawRect(box_offset, box_offset, w, h);
}
void PlotArea::drawGrid(QPainter& p)
{
    QPen gridPen(gridColor);
    gridPen.setWidth(grid_line_width);
    p.setPen(gridPen);
    int i = 0;
    while(zx + i * u <= width() - box_offset)
    {
        i++;
        p.drawLine(zx + i * u, box_offset, zx + i * u, height() - box_offset);
        p.drawLine(zx - i * u, box_offset, zx - i * u, height() - box_offset);
    }
    i = 0;
    while(zy + i * u < height())
    {
        i++;
        p.drawLine(box_offset, zy + i * u, width() - box_offset, zy + i * u);
        p.drawLine(box_offset, zy - i * u, width() - box_offset, zy - i * u);
    }
}
void PlotArea::drawAxis(QPainter& p)
{
    QPen axisPen(axisColor);
    axisPen.setWidth(axis_width);
    p.setPen(axisPen);
    p.drawLine(box_offset, zy, width() - box_offset, zy);
    p.drawLine(zx, box_offset, zx, height() - box_offset);
}
void PlotArea::drawTicks(QPainter& p)
{
    QPen ticksPen(axisColor);
    ticksPen.setWidth(axis_width);
    p.setPen(ticksPen);
    QFont font = p.font();
    font.setPixelSize(12);
    p.setFont(font);
    //ticks x
    int i = 0;
    int alignFlags = Qt::AlignRight | Qt::AlignTop;
    p.drawText(QRect{zx  - u + pixel_width, zy + pixel_width, u - pixel_width, u - pixel_width}, alignFlags, QString::number(0));
    while(zx + (i + 2) * u < width())
    {
        i++;
        p.drawLine(zx + i * u, zy + tick_length, zx + i * u, zy - tick_length);
        p.drawLine(zx - i * u, zy + tick_length, zx - i * u, zy - tick_length);
        if (zx + (i + 1) * u < width())
            p.drawText(QRect{zx + (i - 1) * u + pixel_width, zy + pixel_width, u - pixel_width, u - pixel_width}, alignFlags, QString::number(i));
        p.drawText(QRect{zx - (i + 1) * u + pixel_width, zy + pixel_width, u - pixel_width, u - pixel_width}, alignFlags, QString::number(-i));
    }
    //ticks y
    i = 0;
    //p.drawText(QRect{zx - u +  pixel_width, zy - u + pixel_width, u - pixel_width, u - pixel_width}, QString::number(0));
    while(zy + (i + 2) * u < height())
    {
        i++;
        p.drawLine(zx - tick_length, zy + i * u, zx + tick_length, zy + i * u);
        p.drawLine(zx - tick_length, zy - i * u, zx + tick_length, zy - i * u);
        if (zy - (i + 1) * u > 0)
            p.drawText(QRect{zx  - u + pixel_width, zy - (i) * u +  pixel_width, u - pixel_width, u - pixel_width}, alignFlags, QString::number(i));
        p.drawText(QRect{zx - u + pixel_width, zy + (i) * u + pixel_width, u - pixel_width, u - pixel_width}, alignFlags, QString::number(-i));
    }
}
void PlotArea::drawArrows(QPainter& p)
{
    QPen arrowsPen(axisColor);
    arrowsPen.setWidth((axis_width));
    p.setBrush(QBrush(axisColor));
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
    //arrow x
    QPainterPath px;
    px.moveTo(width() - u - 1, zy + 2 * tick_length);
    px.lineTo(width() - u - 1, zy - 2 * tick_length);
    px.lineTo(width() - 1, zy);
    px.lineTo(width() - u - 1, zy + 2 * tick_length);
    p.drawPath(px);
    p.drawText(QRect{width() - u / 2 - 1, zy + u / 2, u, u}, "X");
    //arrow y
    QPainterPath py;
    py.moveTo(zx + 2 * tick_length, u + 1);
    py.lineTo(zx - 2 * tick_length, u + 1);
    py.lineTo(zx, 1);
    py.lineTo(zx + 2 * tick_length, u + 1);
    p.drawPath(py);
    p.drawText(QRect{zx + u / 2, u / 2, u, u}, "Y");
}
void PlotArea::drawClippingWindow(QPainter& p)
{
    p.setPen(clippingWindowColor);
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRect{Adjust(clippingWindowp1), Adjust(clippingWindowp2)});
}
void PlotArea::drawLineSegments(QPainter& p)
{
    if (segments.empty())
    {
        QMessageBox::warning(nullptr, "Ошибка", "Нет ни одного отрезка");
        return;
    }
    for (const auto& segmentData : segments)
    {
        p.setPen(QPen(segmentData.color(), line_width));
        p.drawLine(Adjust(segmentData.p1()), Adjust(segmentData.p2()));
    }
}
void PlotArea::drawPolygon(QPainter& p)
{
    if (polygonData.empty())
    {
        QMessageBox::warning(nullptr, "Ошибка", "Многоугольник пуст");
        return;
    }
    p.setPen(QPen(polygonBorderColor, line_width));
    p.setBrush(Qt::NoBrush);
    QPainterPath path;
    path.moveTo(Adjust(polygonData[0]));
    for (size_t i = 1; i < polygonData.size(); ++i)
    {
        path.lineTo(Adjust(polygonData[i]));
    }
    path.lineTo(Adjust(polygonData[0]));
    p.drawPath(path);
}
void PlotArea::AddLineSegment(const LineSegmentData& data)
{
    segments.push_back(data);
}
void PlotArea::AddPolygonPoint(int x, int y)
{
    polygonData.push_back({x, y});
}
void PlotArea::SetPolygonBorderColor(const QColor& color)
{
    polygonBorderColor = color;
}
void PlotArea::SetPolygonFillingColor(const QColor& color)
{
    polygonFillingColor = color;
}
void PlotArea::SetClippingWindow(const QPoint& p1, const QPoint& p2)
{
    clippingWindowp1 = p1;
    clippingWindowp2 = p2;
}
void PlotArea::ChangeMode(PlotMode newMode)
{
    mode = newMode;
}
void PlotArea::Clear()
{
    segments.clear();
    polygonData.clear();
}
void PlotArea::paintEvent(QPaintEvent*)
{
    zx = width() / 2;
    zy = height() / 2;
    QPainter pt(this);
    drawBox(pt);
    drawGrid(pt);
    drawAxis(pt);
    drawTicks(pt);
    drawArrows(pt);
    switch(mode)
    {
        case PlotMode::Segments:
            drawLineSegments(pt);
            drawClippingWindow(pt);
            break;
        case PlotMode::Polygons:
            drawPolygon(pt);
            drawClippingWindow(pt);
            break;
        case PlotMode::None:
            break;
    }
}
int PlotArea::getUnit() const
{
    return u;
}
void PlotArea::SetUnit(int nu)
{
      u = nu;
}
