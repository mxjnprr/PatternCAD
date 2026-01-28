/**
 * Line.cpp
 *
 * Implementation of Line class
 */

#include "Line.h"
#include <QPainter>
#include <QtMath>

namespace PatternCAD {
namespace Geometry {

Line::Line(QObject* parent)
    : GeometryObject(parent)
    , m_start(0.0, 0.0)
    , m_end(0.0, 0.0)
{
    m_name = "Line";
}

Line::Line(const QPointF& start, const QPointF& end, QObject* parent)
    : GeometryObject(parent)
    , m_start(start)
    , m_end(end)
{
    m_name = "Line";
}

Line::~Line()
{
}

ObjectType Line::type() const
{
    return ObjectType::Line;
}

QString Line::typeName() const
{
    return "Line";
}

QPointF Line::start() const
{
    return m_start;
}

void Line::setStart(const QPointF& start)
{
    if (m_start != start) {
        m_start = start;
        notifyChanged();
    }
}

QPointF Line::end() const
{
    return m_end;
}

void Line::setEnd(const QPointF& end)
{
    if (m_end != end) {
        m_end = end;
        notifyChanged();
    }
}

void Line::setPoints(const QPointF& start, const QPointF& end)
{
    m_start = start;
    m_end = end;
    notifyChanged();
}

double Line::length() const
{
    QPointF delta = m_end - m_start;
    return qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
}

double Line::angle() const
{
    QLineF line(m_start, m_end);
    return line.angle();
}

QPointF Line::midpoint() const
{
    return (m_start + m_end) / 2.0;
}

double Line::distanceToPoint(const QPointF& point) const
{
    // Calculate perpendicular distance from point to line
    QLineF line(m_start, m_end);

    // Vector from start to end
    QPointF lineVec = m_end - m_start;
    double lineLen = length();

    if (lineLen < 0.0001) {
        // Degenerate line - return distance to start point
        QPointF delta = point - m_start;
        return qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
    }

    // Normalize line vector
    lineVec /= lineLen;

    // Project point onto line
    QPointF pointVec = point - m_start;
    double t = pointVec.x() * lineVec.x() + pointVec.y() * lineVec.y();

    // Clamp to line segment
    t = qBound(0.0, t, lineLen);

    // Find closest point on line segment
    QPointF closest = m_start + lineVec * t;

    // Return distance
    QPointF delta = point - closest;
    return qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
}

QPointF Line::closestPointOnLine(const QPointF& point) const
{
    QPointF lineVec = m_end - m_start;
    double lineLen = length();

    if (lineLen < 0.0001) {
        return m_start;
    }

    lineVec /= lineLen;
    QPointF pointVec = point - m_start;
    double t = pointVec.x() * lineVec.x() + pointVec.y() * lineVec.y();
    t = qBound(0.0, t, lineLen);

    return m_start + lineVec * t;
}

QRectF Line::boundingRect() const
{
    double left = qMin(m_start.x(), m_end.x());
    double top = qMin(m_start.y(), m_end.y());
    double right = qMax(m_start.x(), m_end.x());
    double bottom = qMax(m_start.y(), m_end.y());

    // Add small margin
    double margin = 2.0;
    return QRectF(left - margin, top - margin,
                  right - left + 2 * margin,
                  bottom - top + 2 * margin);
}

bool Line::contains(const QPointF& point) const
{
    return distanceToPoint(point) <= HIT_TOLERANCE;
}

void Line::translate(const QPointF& delta)
{
    m_start += delta;
    m_end += delta;
    notifyChanged();
}

void Line::draw(QPainter* painter, const QColor& color) const
{
    if (!m_visible) {
        return;
    }

    painter->save();

    // Use object's pen properties
    painter->setPen(createPen(color));

    // Draw line
    painter->drawLine(m_start, m_end);

    // Draw endpoints if selected
    if (m_selected) {
        painter->setBrush(Qt::red);
        double pointSize = 4.0;
        painter->drawEllipse(m_start, pointSize, pointSize);
        painter->drawEllipse(m_end, pointSize, pointSize);
    }

    painter->restore();
}

} // namespace Geometry
} // namespace PatternCAD
