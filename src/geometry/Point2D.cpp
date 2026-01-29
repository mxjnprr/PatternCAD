/**
 * Point2D.cpp
 *
 * Implementation of Point2D class
 */

#include "Point2D.h"
#include <QPainter>
#include <QtMath>

namespace PatternCAD {
namespace Geometry {

Point2D::Point2D(QObject* parent)
    : GeometryObject(parent)
    , m_position(0.0, 0.0)
{
    m_name = "Point";
}

Point2D::Point2D(const QPointF& position, QObject* parent)
    : GeometryObject(parent)
    , m_position(position)
{
    m_name = "Point";
}

Point2D::Point2D(double x, double y, QObject* parent)
    : GeometryObject(parent)
    , m_position(x, y)
{
    m_name = "Point";
}

Point2D::~Point2D()
{
}

ObjectType Point2D::type() const
{
    return ObjectType::Point;
}

QString Point2D::typeName() const
{
    return "Point";
}

QPointF Point2D::position() const
{
    return m_position;
}

void Point2D::setPosition(const QPointF& position)
{
    if (m_position != position) {
        m_position = position;
        notifyChanged();
    }
}

void Point2D::setPosition(double x, double y)
{
    setPosition(QPointF(x, y));
}

double Point2D::x() const
{
    return m_position.x();
}

void Point2D::setX(double x)
{
    if (m_position.x() != x) {
        m_position.setX(x);
        notifyChanged();
    }
}

double Point2D::y() const
{
    return m_position.y();
}

void Point2D::setY(double y)
{
    if (m_position.y() != y) {
        m_position.setY(y);
        notifyChanged();
    }
}

double Point2D::distanceTo(const QPointF& other) const
{
    QPointF delta = m_position - other;
    return qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
}

double Point2D::distanceTo(const Point2D* other) const
{
    if (!other) {
        return 0.0;
    }
    return distanceTo(other->position());
}

QRectF Point2D::boundingRect() const
{
    return QRectF(m_position.x() - POINT_SIZE / 2,
                  m_position.y() - POINT_SIZE / 2,
                  POINT_SIZE,
                  POINT_SIZE);
}

bool Point2D::contains(const QPointF& point) const
{
    return distanceTo(point) <= HIT_TOLERANCE;
}

void Point2D::translate(const QPointF& delta)
{
    setPosition(m_position + delta);
}

void Point2D::rotate(double angleDegrees, const QPointF& center)
{
    // Convert angle to radians
    double angleRadians = qDegreesToRadians(angleDegrees);
    double cosAngle = qCos(angleRadians);
    double sinAngle = qSin(angleRadians);

    // Translate to origin
    double dx = m_position.x() - center.x();
    double dy = m_position.y() - center.y();

    // Rotate
    double rotatedX = dx * cosAngle - dy * sinAngle;
    double rotatedY = dx * sinAngle + dy * cosAngle;

    // Translate back
    setPosition(center.x() + rotatedX, center.y() + rotatedY);
}

void Point2D::mirror(const QPointF& axisPoint1, const QPointF& axisPoint2)
{
    // Direction vector of the mirror axis
    double dx = axisPoint2.x() - axisPoint1.x();
    double dy = axisPoint2.y() - axisPoint1.y();

    // Normalize the direction vector
    double length = qSqrt(dx * dx + dy * dy);
    if (length < 1e-10) {
        // Degenerate axis (two points are the same), no mirroring
        return;
    }
    double ux = dx / length;
    double uy = dy / length;

    // Vector from axis point to our point
    double vx = m_position.x() - axisPoint1.x();
    double vy = m_position.y() - axisPoint1.y();

    // Project onto axis
    double projection = vx * ux + vy * uy;

    // Closest point on axis
    double closestX = axisPoint1.x() + projection * ux;
    double closestY = axisPoint1.y() + projection * uy;

    // Mirror across the closest point
    double mirroredX = 2.0 * closestX - m_position.x();
    double mirroredY = 2.0 * closestY - m_position.y();

    setPosition(mirroredX, mirroredY);
}

void Point2D::draw(QPainter* painter, const QColor& color) const
{
    if (!m_visible) {
        return;
    }

    painter->save();

    // Set color based on selection state
    QColor drawColor = m_selected ? Qt::red : color;
    painter->setPen(QPen(drawColor, 2));
    painter->setBrush(drawColor);

    // Draw point as small circle
    painter->drawEllipse(m_position, POINT_SIZE / 2, POINT_SIZE / 2);

    painter->restore();
}

} // namespace Geometry
} // namespace PatternCAD
