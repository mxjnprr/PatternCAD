/**
 * Rectangle.cpp
 *
 * Implementation of Rectangle class
 */

#include "Rectangle.h"
#include <QPainter>
#include <QtMath>

namespace PatternCAD {
namespace Geometry {

Rectangle::Rectangle(QObject* parent)
    : GeometryObject(parent)
    , m_topLeft(0.0, 0.0)
    , m_width(100.0)
    , m_height(50.0)
{
    m_name = "Rectangle";
}

Rectangle::Rectangle(const QPointF& topLeft, double width, double height, QObject* parent)
    : GeometryObject(parent)
    , m_topLeft(topLeft)
    , m_width(qAbs(width))
    , m_height(qAbs(height))
{
    m_name = "Rectangle";
}

Rectangle::Rectangle(const QRectF& rect, QObject* parent)
    : GeometryObject(parent)
    , m_topLeft(rect.topLeft())
    , m_width(rect.width())
    , m_height(rect.height())
{
    m_name = "Rectangle";
}

Rectangle::~Rectangle()
{
}

ObjectType Rectangle::type() const
{
    return ObjectType::Rectangle;
}

QString Rectangle::typeName() const
{
    return "Rectangle";
}

QPointF Rectangle::topLeft() const
{
    return m_topLeft;
}

void Rectangle::setTopLeft(const QPointF& topLeft)
{
    if (m_topLeft != topLeft) {
        m_topLeft = topLeft;
        notifyChanged();
    }
}

QPointF Rectangle::topRight() const
{
    return QPointF(m_topLeft.x() + m_width, m_topLeft.y());
}

QPointF Rectangle::bottomLeft() const
{
    return QPointF(m_topLeft.x(), m_topLeft.y() + m_height);
}

QPointF Rectangle::bottomRight() const
{
    return QPointF(m_topLeft.x() + m_width, m_topLeft.y() + m_height);
}

QPointF Rectangle::center() const
{
    return QPointF(m_topLeft.x() + m_width / 2.0,
                   m_topLeft.y() + m_height / 2.0);
}

double Rectangle::width() const
{
    return m_width;
}

void Rectangle::setWidth(double width)
{
    double newWidth = qAbs(width);
    if (m_width != newWidth) {
        m_width = newWidth;
        notifyChanged();
    }
}

double Rectangle::height() const
{
    return m_height;
}

void Rectangle::setHeight(double height)
{
    double newHeight = qAbs(height);
    if (m_height != newHeight) {
        m_height = newHeight;
        notifyChanged();
    }
}

void Rectangle::setSize(double width, double height)
{
    m_width = qAbs(width);
    m_height = qAbs(height);
    notifyChanged();
}

double Rectangle::area() const
{
    return m_width * m_height;
}

double Rectangle::perimeter() const
{
    return 2.0 * (m_width + m_height);
}

QRectF Rectangle::rect() const
{
    return QRectF(m_topLeft, QSizeF(m_width, m_height));
}

void Rectangle::setRect(const QRectF& rect)
{
    m_topLeft = rect.topLeft();
    m_width = rect.width();
    m_height = rect.height();
    notifyChanged();
}

QRectF Rectangle::boundingRect() const
{
    return rect();
}

bool Rectangle::contains(const QPointF& point) const
{
    // Check if point is inside rectangle or on border within tolerance
    QRectF innerRect = rect().adjusted(HIT_TOLERANCE, HIT_TOLERANCE,
                                       -HIT_TOLERANCE, -HIT_TOLERANCE);
    QRectF outerRect = rect().adjusted(-HIT_TOLERANCE, -HIT_TOLERANCE,
                                       HIT_TOLERANCE, HIT_TOLERANCE);

    // Point must be in outer rect but not in inner rect (on border)
    // or we accept any point within outer rect for simplicity
    return outerRect.contains(point) && !innerRect.contains(point);
}

void Rectangle::translate(const QPointF& delta)
{
    setTopLeft(m_topLeft + delta);
}

void Rectangle::draw(QPainter* painter) const
{
    if (!m_visible) {
        return;
    }

    painter->save();

    // Set color and style based on selection state
    QColor color = m_selected ? Qt::red : Qt::black;
    QPen pen(color, m_selected ? 2 : 1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw rectangle
    painter->drawRect(rect());

    // Draw corner points if selected
    if (m_selected) {
        painter->setBrush(color);
        double pointSize = 4.0;
        painter->drawEllipse(topLeft(), pointSize, pointSize);
        painter->drawEllipse(topRight(), pointSize, pointSize);
        painter->drawEllipse(bottomLeft(), pointSize, pointSize);
        painter->drawEllipse(bottomRight(), pointSize, pointSize);
    }

    painter->restore();
}

} // namespace Geometry
} // namespace PatternCAD
