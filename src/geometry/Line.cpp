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

namespace {
    // Helper function to rotate a point around a center
    QPointF rotatePoint(const QPointF& point, double angleDegrees, const QPointF& center) {
        double angleRadians = qDegreesToRadians(angleDegrees);
        double cosAngle = qCos(angleRadians);
        double sinAngle = qSin(angleRadians);

        double dx = point.x() - center.x();
        double dy = point.y() - center.y();

        double rotatedX = dx * cosAngle - dy * sinAngle;
        double rotatedY = dx * sinAngle + dy * cosAngle;

        return QPointF(center.x() + rotatedX, center.y() + rotatedY);
    }

    // Helper function to mirror a point across an axis
    QPointF mirrorPoint(const QPointF& point, const QPointF& axisPoint1, const QPointF& axisPoint2) {
        double dx = axisPoint2.x() - axisPoint1.x();
        double dy = axisPoint2.y() - axisPoint1.y();

        double length = qSqrt(dx * dx + dy * dy);
        if (length < 1e-10) {
            return point;
        }
        double ux = dx / length;
        double uy = dy / length;

        double vx = point.x() - axisPoint1.x();
        double vy = point.y() - axisPoint1.y();

        double projection = vx * ux + vy * uy;

        double closestX = axisPoint1.x() + projection * ux;
        double closestY = axisPoint1.y() + projection * uy;

        return QPointF(2.0 * closestX - point.x(), 2.0 * closestY - point.y());
    }

    // Helper function to scale a point around an origin
    QPointF scalePoint(const QPointF& point, double scaleX, double scaleY, const QPointF& origin) {
        double dx = point.x() - origin.x();
        double dy = point.y() - origin.y();

        double scaledX = dx * scaleX;
        double scaledY = dy * scaleY;

        return QPointF(origin.x() + scaledX, origin.y() + scaledY);
    }
}

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

void Line::rotate(double angleDegrees, const QPointF& center)
{
    m_start = rotatePoint(m_start, angleDegrees, center);
    m_end = rotatePoint(m_end, angleDegrees, center);
    notifyChanged();
}

void Line::mirror(const QPointF& axisPoint1, const QPointF& axisPoint2)
{
    m_start = mirrorPoint(m_start, axisPoint1, axisPoint2);
    m_end = mirrorPoint(m_end, axisPoint1, axisPoint2);
    notifyChanged();
}

void Line::scale(double scaleX, double scaleY, const QPointF& origin)
{
    m_start = scalePoint(m_start, scaleX, scaleY, origin);
    m_end = scalePoint(m_end, scaleX, scaleY, origin);
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
