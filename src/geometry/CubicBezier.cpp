/**
 * CubicBezier.cpp
 *
 * Implementation of CubicBezier class
 */

#include "CubicBezier.h"
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
}

CubicBezier::CubicBezier(QObject* parent)
    : GeometryObject(parent)
    , m_p0(0.0, 0.0)
    , m_p1(50.0, 100.0)
    , m_p2(150.0, 100.0)
    , m_p3(200.0, 0.0)
{
    m_name = "Bezier Curve";
}

CubicBezier::CubicBezier(const QPointF& p0, const QPointF& p1,
                         const QPointF& p2, const QPointF& p3,
                         QObject* parent)
    : GeometryObject(parent)
    , m_p0(p0)
    , m_p1(p1)
    , m_p2(p2)
    , m_p3(p3)
{
    m_name = "Bezier Curve";
}

CubicBezier::~CubicBezier()
{
}

ObjectType CubicBezier::type() const
{
    return ObjectType::CubicBezier;
}

QString CubicBezier::typeName() const
{
    return "Cubic Bezier";
}

QPointF CubicBezier::p0() const
{
    return m_p0;
}

void CubicBezier::setP0(const QPointF& p0)
{
    if (m_p0 != p0) {
        m_p0 = p0;
        notifyChanged();
    }
}

QPointF CubicBezier::p1() const
{
    return m_p1;
}

void CubicBezier::setP1(const QPointF& p1)
{
    if (m_p1 != p1) {
        m_p1 = p1;
        notifyChanged();
    }
}

QPointF CubicBezier::p2() const
{
    return m_p2;
}

void CubicBezier::setP2(const QPointF& p2)
{
    if (m_p2 != p2) {
        m_p2 = p2;
        notifyChanged();
    }
}

QPointF CubicBezier::p3() const
{
    return m_p3;
}

void CubicBezier::setP3(const QPointF& p3)
{
    if (m_p3 != p3) {
        m_p3 = p3;
        notifyChanged();
    }
}

void CubicBezier::setPoints(const QPointF& p0, const QPointF& p1,
                            const QPointF& p2, const QPointF& p3)
{
    m_p0 = p0;
    m_p1 = p1;
    m_p2 = p2;
    m_p3 = p3;
    notifyChanged();
}

QPointF CubicBezier::pointAt(double t) const
{
    // Clamp t to [0, 1]
    t = qBound(0.0, t, 1.0);

    // Cubic Bezier formula: B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
    double u = 1.0 - t;
    double u2 = u * u;
    double u3 = u2 * u;
    double t2 = t * t;
    double t3 = t2 * t;

    double x = u3 * m_p0.x() +
               3.0 * u2 * t * m_p1.x() +
               3.0 * u * t2 * m_p2.x() +
               t3 * m_p3.x();

    double y = u3 * m_p0.y() +
               3.0 * u2 * t * m_p1.y() +
               3.0 * u * t2 * m_p2.y() +
               t3 * m_p3.y();

    return QPointF(x, y);
}

QPointF CubicBezier::tangentAt(double t) const
{
    // Derivative of cubic Bezier: B'(t) = 3(1-t)²(P1-P0) + 6(1-t)t(P2-P1) + 3t²(P3-P2)
    t = qBound(0.0, t, 1.0);

    double u = 1.0 - t;
    double u2 = u * u;
    double t2 = t * t;

    QPointF d1 = m_p1 - m_p0;
    QPointF d2 = m_p2 - m_p1;
    QPointF d3 = m_p3 - m_p2;

    double x = 3.0 * u2 * d1.x() + 6.0 * u * t * d2.x() + 3.0 * t2 * d3.x();
    double y = 3.0 * u2 * d1.y() + 6.0 * u * t * d2.y() + 3.0 * t2 * d3.y();

    return QPointF(x, y);
}

double CubicBezier::length() const
{
    return approximateLength();
}

double CubicBezier::approximateLength() const
{
    // Approximate length by sampling points along curve
    double totalLength = 0.0;
    QPointF prevPoint = m_p0;

    for (int i = 1; i <= SAMPLE_COUNT; ++i) {
        double t = static_cast<double>(i) / SAMPLE_COUNT;
        QPointF currentPoint = pointAt(t);
        QPointF delta = currentPoint - prevPoint;
        totalLength += qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
        prevPoint = currentPoint;
    }

    return totalLength;
}

QPainterPath CubicBezier::createPath() const
{
    QPainterPath path;
    path.moveTo(m_p0);
    path.cubicTo(m_p1, m_p2, m_p3);
    return path;
}

QRectF CubicBezier::boundingRect() const
{
    QPainterPath path = createPath();
    return path.boundingRect();
}

bool CubicBezier::contains(const QPointF& point) const
{
    // Check if point is close to curve
    double minDistance = std::numeric_limits<double>::max();

    for (int i = 0; i <= SAMPLE_COUNT; ++i) {
        double t = static_cast<double>(i) / SAMPLE_COUNT;
        QPointF curvePoint = pointAt(t);
        QPointF delta = point - curvePoint;
        double distance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
        minDistance = qMin(minDistance, distance);
    }

    return minDistance <= HIT_TOLERANCE;
}

void CubicBezier::translate(const QPointF& delta)
{
    m_p0 += delta;
    m_p1 += delta;
    m_p2 += delta;
    m_p3 += delta;
    notifyChanged();
}

void CubicBezier::rotate(double angleDegrees, const QPointF& center)
{
    m_p0 = rotatePoint(m_p0, angleDegrees, center);
    m_p1 = rotatePoint(m_p1, angleDegrees, center);
    m_p2 = rotatePoint(m_p2, angleDegrees, center);
    m_p3 = rotatePoint(m_p3, angleDegrees, center);
    notifyChanged();
}

void CubicBezier::draw(QPainter* painter, const QColor& color) const
{
    if (!m_visible) {
        return;
    }

    painter->save();

    // Set color and style based on selection state
    QColor drawColor = m_selected ? Qt::red : color;
    QPen pen(drawColor, m_selected ? 2 : 1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw curve
    QPainterPath path = createPath();
    painter->drawPath(path);

    // Draw control points and control polygon if selected
    if (m_selected) {
        // Control polygon
        QPen controlPen(Qt::blue, 1, Qt::DashLine);
        painter->setPen(controlPen);
        painter->drawLine(m_p0, m_p1);
        painter->drawLine(m_p2, m_p3);

        // Control points
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::blue);
        double pointSize = 4.0;
        painter->drawEllipse(m_p0, pointSize, pointSize);
        painter->drawEllipse(m_p3, pointSize, pointSize);

        // Control handles
        painter->setBrush(QColor(100, 149, 237)); // Cornflower blue
        painter->drawEllipse(m_p1, pointSize, pointSize);
        painter->drawEllipse(m_p2, pointSize, pointSize);
    }

    painter->restore();
}

} // namespace Geometry
} // namespace PatternCAD
