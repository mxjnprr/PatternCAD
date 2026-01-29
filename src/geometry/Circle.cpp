/**
 * Circle.cpp
 *
 * Implementation of Circle class
 */

#include "Circle.h"
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
}

Circle::Circle(QObject* parent)
    : GeometryObject(parent)
    , m_center(0.0, 0.0)
    , m_radius(10.0)
{
    m_name = "Circle";
}

Circle::Circle(const QPointF& center, double radius, QObject* parent)
    : GeometryObject(parent)
    , m_center(center)
    , m_radius(qAbs(radius))
{
    m_name = "Circle";
}

Circle::~Circle()
{
}

ObjectType Circle::type() const
{
    return ObjectType::Circle;
}

QString Circle::typeName() const
{
    return "Circle";
}

QPointF Circle::center() const
{
    return m_center;
}

void Circle::setCenter(const QPointF& center)
{
    if (m_center != center) {
        m_center = center;
        notifyChanged();
    }
}

double Circle::radius() const
{
    return m_radius;
}

void Circle::setRadius(double radius)
{
    double newRadius = qAbs(radius);
    if (m_radius != newRadius) {
        m_radius = newRadius;
        notifyChanged();
    }
}

double Circle::diameter() const
{
    return m_radius * 2.0;
}

double Circle::area() const
{
    return M_PI * m_radius * m_radius;
}

double Circle::circumference() const
{
    return 2.0 * M_PI * m_radius;
}

bool Circle::containsPoint(const QPointF& point) const
{
    QPointF delta = point - m_center;
    double distance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
    return distance <= m_radius;
}

double Circle::distanceToPoint(const QPointF& point) const
{
    QPointF delta = point - m_center;
    double distance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
    return qAbs(distance - m_radius);
}

QRectF Circle::boundingRect() const
{
    return QRectF(m_center.x() - m_radius,
                  m_center.y() - m_radius,
                  m_radius * 2.0,
                  m_radius * 2.0);
}

bool Circle::contains(const QPointF& point) const
{
    return distanceToPoint(point) <= HIT_TOLERANCE;
}

void Circle::translate(const QPointF& delta)
{
    setCenter(m_center + delta);
}

void Circle::rotate(double angleDegrees, const QPointF& center)
{
    // For a circle, we only need to rotate its center point
    // The radius remains the same
    setCenter(rotatePoint(m_center, angleDegrees, center));
}

void Circle::mirror(const QPointF& axisPoint1, const QPointF& axisPoint2)
{
    // For a circle, we only need to mirror its center point
    // The radius remains the same
    setCenter(mirrorPoint(m_center, axisPoint1, axisPoint2));
}

void Circle::draw(QPainter* painter, const QColor& color) const
{
    if (!m_visible) {
        return;
    }

    painter->save();

    // Use object's pen properties
    painter->setPen(createPen(color));
    painter->setBrush(Qt::NoBrush);

    // Draw circle
    painter->drawEllipse(m_center, m_radius, m_radius);

    // Draw center point if selected
    if (m_selected) {
        painter->setBrush(Qt::red);
        double pointSize = 4.0;
        painter->drawEllipse(m_center, pointSize, pointSize);
    }

    painter->restore();
}

} // namespace Geometry
} // namespace PatternCAD
