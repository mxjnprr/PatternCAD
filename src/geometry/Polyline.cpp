/**
 * Polyline.cpp
 *
 * Implementation of Polyline
 */

#include "Polyline.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <cmath>
#include <limits>

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

    // Helper to rotate a vector (for tangent rotation, no translation)
    QPointF rotateVector(const QPointF& vec, double angleDegrees) {
        double angleRadians = qDegreesToRadians(angleDegrees);
        double cosAngle = qCos(angleRadians);
        double sinAngle = qSin(angleRadians);

        double rotatedX = vec.x() * cosAngle - vec.y() * sinAngle;
        double rotatedY = vec.x() * sinAngle + vec.y() * cosAngle;

        return QPointF(rotatedX, rotatedY);
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

    // Helper to mirror a vector (for tangent mirroring, no translation)
    QPointF mirrorVector(const QPointF& vec, const QPointF& axisPoint1, const QPointF& axisPoint2) {
        double dx = axisPoint2.x() - axisPoint1.x();
        double dy = axisPoint2.y() - axisPoint1.y();

        double length = qSqrt(dx * dx + dy * dy);
        if (length < 1e-10) {
            return vec;
        }
        double ux = dx / length;
        double uy = dy / length;

        // Project vector onto axis
        double projection = vec.x() * ux + vec.y() * uy;

        // Mirror the vector
        double mirroredX = 2.0 * projection * ux - vec.x();
        double mirroredY = 2.0 * projection * uy - vec.y();

        return QPointF(mirroredX, mirroredY);
    }
}

Polyline::Polyline(QObject* parent)
    : GeometryObject(parent)
    , m_closed(true)
{
}

Polyline::Polyline(const QVector<PolylineVertex>& vertices, QObject* parent)
    : GeometryObject(parent)
    , m_vertices(vertices)
    , m_closed(true)
{
}

Polyline::~Polyline()
{
}

void Polyline::setVertices(const QVector<PolylineVertex>& vertices)
{
    m_vertices = vertices;
    notifyChanged();
}

void Polyline::addVertex(const QPointF& position, VertexType type, double tension, const QPointF& tangent)
{
    m_vertices.append(PolylineVertex(position, type, tension, tangent));
    notifyChanged();
}

void Polyline::addVertex(const PolylineVertex& vertex)
{
    m_vertices.append(vertex);
    notifyChanged();
}

void Polyline::setClosed(bool closed)
{
    if (m_closed != closed) {
        m_closed = closed;
        notifyChanged();
    }
}

QRectF Polyline::boundingRect() const
{
    if (m_vertices.isEmpty()) {
        return QRectF();
    }

    qreal minX = m_vertices[0].position.x();
    qreal minY = m_vertices[0].position.y();
    qreal maxX = minX;
    qreal maxY = minY;

    for (const auto& vertex : m_vertices) {
        minX = qMin(minX, vertex.position.x());
        minY = qMin(minY, vertex.position.y());
        maxX = qMax(maxX, vertex.position.x());
        maxY = qMax(maxY, vertex.position.y());
    }

    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

bool Polyline::contains(const QPointF& point) const
{
    QPainterPath path = createPath();
    return path.contains(point);
}

void Polyline::translate(const QPointF& delta)
{
    for (auto& vertex : m_vertices) {
        vertex.position += delta;
    }
    notifyChanged();
}

void Polyline::rotate(double angleDegrees, const QPointF& center)
{
    for (auto& vertex : m_vertices) {
        // Rotate the position
        vertex.position = rotatePoint(vertex.position, angleDegrees, center);
        // Rotate the tangent vector (if it exists)
        if (vertex.tangent != QPointF()) {
            vertex.tangent = rotateVector(vertex.tangent, angleDegrees);
        }
    }
    notifyChanged();
}

void Polyline::mirror(const QPointF& axisPoint1, const QPointF& axisPoint2)
{
    for (auto& vertex : m_vertices) {
        // Mirror the position
        vertex.position = mirrorPoint(vertex.position, axisPoint1, axisPoint2);
        // Mirror the tangent vector (if it exists)
        if (vertex.tangent != QPointF()) {
            vertex.tangent = mirrorVector(vertex.tangent, axisPoint1, axisPoint2);
        }
    }
    notifyChanged();
}

void Polyline::draw(QPainter* painter, const QColor& color) const
{
    if (m_vertices.size() < 2) {
        return;
    }

    painter->save();

    // Set pen style - use layer color unless selected
    QPen pen(m_selected ? Qt::blue : color);
    pen.setWidth(m_selected ? 2 : 1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw the path
    QPainterPath path = createPath();
    painter->drawPath(path);

    // Draw vertex markers if selected
    if (m_selected) {
        for (const auto& vertex : m_vertices) {
            QColor vertexColor = (vertex.type == VertexType::Sharp) ? Qt::red : Qt::green;
            painter->setBrush(vertexColor);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(vertex.position, 3, 3);
        }
    }

    painter->restore();
}

QPainterPath Polyline::createPath() const
{
    QPainterPath path;

    if (m_vertices.isEmpty()) {
        return path;
    }

    int n = m_vertices.size();

    // Start at the first vertex
    path.moveTo(m_vertices[0].position);

    // Draw segments between vertices
    for (int i = 0; i < n; ++i) {
        int nextIdx = (i + 1) % n;

        // If we're at the last vertex and not closed, don't draw to first
        if (!m_closed && i == n - 1) {
            break;
        }

        const PolylineVertex& current = m_vertices[i];
        const PolylineVertex& next = m_vertices[nextIdx];

        // A segment is curved if EITHER endpoint is smooth
        bool needsCurve = (current.type == VertexType::Smooth) ||
                          (next.type == VertexType::Smooth);

        if (needsCurve) {
            QPointF p1 = current.position;
            QPointF p2 = next.position;

            // Distance between points for scaling control points
            QPointF segment = p2 - p1;
            double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
            double controlDistance = dist / 3.0;

            // Calculate control points for cubic Bezier
            QPointF c1, c2;

            // Determine control point c1 (outgoing from current)
            if (current.type == VertexType::Smooth && current.tangent != QPointF()) {
                // Current is smooth with explicit tangent
                c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
            } else if (current.type == VertexType::Smooth) {
                // Current is smooth: use Catmull-Rom
                int prevIdx = (i - 1 + n) % n;
                QPointF p0 = m_vertices[prevIdx].position;
                if (!m_closed && i == 0) p0 = p1;
                c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
            } else {
                // Current is sharp: place control point very close to p1 for sharp angle
                c1 = p1 + (p2 - p1) * 0.01;
            }

            // Determine control point c2 (incoming to next)
            if (next.type == VertexType::Smooth && next.tangent != QPointF()) {
                // Next is smooth with explicit tangent - symmetric control
                c2 = p2 - next.tangent * controlDistance * next.incomingTension;
            } else if (next.type == VertexType::Smooth) {
                // Next is smooth: use Catmull-Rom
                int nextNextIdx = (i + 2) % n;
                QPointF p3 = m_vertices[nextNextIdx].position;
                if (!m_closed && nextIdx == n - 1) p3 = p2;
                c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
            } else {
                // Next is sharp: place control point very close to p2 for sharp angle
                c2 = p2 - (p2 - p1) * 0.01;
            }

            path.cubicTo(c1, c2, p2);
        } else {
            // Both endpoints are sharp: straight line
            path.lineTo(next.position);
        }
    }

    // Close the path if needed
    if (m_closed && n > 2) {
        path.closeSubpath();
    }

    return path;
}

// Helper function to calculate length of a cubic Bezier curve
static double bezierLength(const QPointF& p0, const QPointF& c1, const QPointF& c2, const QPointF& p1)
{
    // Use adaptive subdivision to approximate the curve length
    // Simpson's rule integration with recursive subdivision
    const int numSegments = 20; // Number of linear segments to approximate the curve
    double length = 0.0;

    for (int i = 0; i < numSegments; ++i) {
        double t1 = static_cast<double>(i) / numSegments;
        double t2 = static_cast<double>(i + 1) / numSegments;

        // Calculate points on the Bezier curve at t1 and t2
        auto bezierPoint = [&](double t) -> QPointF {
            double mt = 1.0 - t;
            double mt2 = mt * mt;
            double mt3 = mt2 * mt;
            double t2 = t * t;
            double t3 = t2 * t;

            return QPointF(
                mt3 * p0.x() + 3.0 * mt2 * t * c1.x() + 3.0 * mt * t2 * c2.x() + t3 * p1.x(),
                mt3 * p0.y() + 3.0 * mt2 * t * c1.y() + 3.0 * mt * t2 * c2.y() + t3 * p1.y()
            );
        };

        QPointF pt1 = bezierPoint(t1);
        QPointF pt2 = bezierPoint(t2);

        QPointF delta = pt2 - pt1;
        length += std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    }

    return length;
}

double Polyline::calculateSegmentLength(int segmentIndex) const
{
    if (segmentIndex < 0 || segmentIndex >= m_vertices.size()) {
        return 0.0;
    }

    int n = m_vertices.size();
    int nextIdx = (segmentIndex + 1) % n;

    if (!m_closed && segmentIndex == n - 1) {
        return 0.0; // No segment after last vertex in open polyline
    }

    const PolylineVertex& current = m_vertices[segmentIndex];
    const PolylineVertex& next = m_vertices[nextIdx];

    QPointF p1 = current.position;
    QPointF p2 = next.position;

    // Check if this segment is curved
    bool needsCurve = (current.type == VertexType::Smooth) ||
                      (next.type == VertexType::Smooth);

    if (!needsCurve) {
        // Straight line
        QPointF delta = p2 - p1;
        return std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    }

    // Calculate control points for curved segment
    QPointF segment = p2 - p1;
    double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
    double controlDistance = dist / 3.0;

    QPointF c1, c2;

    // Control point c1 (outgoing from current)
    if (current.type == VertexType::Smooth && current.tangent != QPointF()) {
        c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
    } else if (current.type == VertexType::Smooth) {
        int prevIdx = (segmentIndex - 1 + n) % n;
        QPointF p0 = m_vertices[prevIdx].position;
        if (!m_closed && segmentIndex == 0) p0 = p1;
        c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
    } else {
        c1 = p1 + (p2 - p1) * 0.01;
    }

    // Control point c2 (incoming to next)
    if (next.type == VertexType::Smooth && next.tangent != QPointF()) {
        c2 = p2 - next.tangent * controlDistance * next.incomingTension;
    } else if (next.type == VertexType::Smooth) {
        int nextNextIdx = (segmentIndex + 2) % n;
        QPointF p3 = m_vertices[nextNextIdx].position;
        if (!m_closed && nextIdx == n - 1) p3 = p2;
        c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
    } else {
        c2 = p2 - (p2 - p1) * 0.01;
    }

    // Calculate the Bezier curve length
    return bezierLength(p1, c1, c2, p2);
}

double Polyline::calculateSegmentLength(int segmentIndex, int overrideVertexIndex, const QPointF& overridePosition) const
{
    if (segmentIndex < 0 || segmentIndex >= m_vertices.size()) {
        return 0.0;
    }

    int n = m_vertices.size();
    int nextIdx = (segmentIndex + 1) % n;

    if (!m_closed && segmentIndex == n - 1) {
        return 0.0;
    }

    // Get vertices with override
    auto getVertex = [&](int idx) -> PolylineVertex {
        if (idx == overrideVertexIndex) {
            PolylineVertex v = m_vertices[idx];
            v.position = overridePosition;
            return v;
        }
        return m_vertices[idx];
    };

    const PolylineVertex current = getVertex(segmentIndex);
    const PolylineVertex next = getVertex(nextIdx);

    QPointF p1 = current.position;
    QPointF p2 = next.position;

    // Check if this segment is curved
    bool needsCurve = (current.type == VertexType::Smooth) ||
                      (next.type == VertexType::Smooth);

    if (!needsCurve) {
        // Straight line
        QPointF delta = p2 - p1;
        return std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    }

    // Calculate control points for curved segment
    QPointF segment = p2 - p1;
    double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
    double controlDistance = dist / 3.0;

    QPointF c1, c2;

    // Control point c1
    if (current.type == VertexType::Smooth && current.tangent != QPointF()) {
        c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
    } else if (current.type == VertexType::Smooth) {
        int prevIdx = (segmentIndex - 1 + n) % n;
        QPointF p0 = getVertex(prevIdx).position;
        if (!m_closed && segmentIndex == 0) p0 = p1;
        c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
    } else {
        c1 = p1 + (p2 - p1) * 0.01;
    }

    // Control point c2
    if (next.type == VertexType::Smooth && next.tangent != QPointF()) {
        c2 = p2 - next.tangent * controlDistance * next.incomingTension;
    } else if (next.type == VertexType::Smooth) {
        int nextNextIdx = (segmentIndex + 2) % n;
        QPointF p3 = getVertex(nextNextIdx).position;
        if (!m_closed && nextIdx == n - 1) p3 = p2;
        c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
    } else {
        c2 = p2 - (p2 - p1) * 0.01;
    }

    return bezierLength(p1, c1, c2, p2);
}

void Polyline::insertVertex(int index, const PolylineVertex& vertex)
{
    if (index >= 0 && index <= m_vertices.size()) {
        m_vertices.insert(index, vertex);
        notifyChanged();
    }
}

void Polyline::removeVertex(int index)
{
    if (index >= 0 && index < m_vertices.size() && m_vertices.size() > 3) {
        // Keep at least 3 vertices for a valid polyline
        m_vertices.remove(index);
        notifyChanged();
    }
}

void Polyline::updateVertex(int index, const QPointF& position)
{
    if (index >= 0 && index < m_vertices.size()) {
        m_vertices[index].position = position;
        notifyChanged();
    }
}

PolylineVertex Polyline::vertexAt(int index) const
{
    if (index >= 0 && index < m_vertices.size()) {
        return m_vertices[index];
    }
    return PolylineVertex();
}

int Polyline::findVertexAt(const QPointF& point, double tolerance) const
{
    for (int i = 0; i < m_vertices.size(); ++i) {
        QPointF delta = m_vertices[i].position - point;
        double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        if (dist <= tolerance) {
            return i;
        }
    }
    return -1;
}

int Polyline::findClosestSegment(const QPointF& point, QPointF* closestPoint) const
{
    if (m_vertices.size() < 2) {
        return -1;
    }

    int closestSegment = -1;
    double minDistance = std::numeric_limits<double>::max();
    QPointF bestPoint;

    int n = m_vertices.size();
    int numSegments = m_closed ? n : (n - 1);

    for (int i = 0; i < numSegments; ++i) {
        int nextIdx = (i + 1) % n;
        const PolylineVertex& current = m_vertices[i];
        const PolylineVertex& next = m_vertices[nextIdx];

        // Check if this segment is curved
        bool needsCurve = (current.type == VertexType::Smooth) ||
                          (next.type == VertexType::Smooth);

        QPointF segmentClosestPoint;
        double distance;

        if (needsCurve) {
            // For curved segments, sample points along the Bezier curve
            QPointF p1 = current.position;
            QPointF p2 = next.position;

            // Calculate Bezier control points (same logic as in createPath)
            QPointF segment = p2 - p1;
            double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
            double controlDistance = dist / 3.0;

            QPointF c1, c2;

            // Control point c1 (outgoing from current)
            if (current.type == VertexType::Smooth && current.tangent != QPointF()) {
                c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
            } else if (current.type == VertexType::Smooth) {
                int prevIdx = (i - 1 + n) % n;
                QPointF p0 = m_vertices[prevIdx].position;
                if (!m_closed && i == 0) p0 = p1;
                c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
            } else {
                c1 = p1 + (p2 - p1) * 0.01;
            }

            // Control point c2 (incoming to next)
            if (next.type == VertexType::Smooth && next.tangent != QPointF()) {
                c2 = p2 - next.tangent * controlDistance * next.incomingTension;
            } else if (next.type == VertexType::Smooth) {
                int nextNextIdx = (i + 2) % n;
                QPointF p3 = m_vertices[nextNextIdx].position;
                if (!m_closed && nextIdx == n - 1) p3 = p2;
                c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
            } else {
                c2 = p2 - (p2 - p1) * 0.01;
            }

            // Sample points along the Bezier curve
            double minDistOnCurve = std::numeric_limits<double>::max();
            QPointF closestOnCurve;

            const int samples = 20;  // Number of samples along the curve
            for (int s = 0; s <= samples; ++s) {
                double t = static_cast<double>(s) / samples;

                // Cubic Bezier formula: B(t) = (1-t)³P0 + 3(1-t)²t*C1 + 3(1-t)t²*C2 + t³*P1
                double u = 1.0 - t;
                double tt = t * t;
                double uu = u * u;
                double uuu = uu * u;
                double ttt = tt * t;

                QPointF curvePoint = uuu * p1 +
                                     3.0 * uu * t * c1 +
                                     3.0 * u * tt * c2 +
                                     ttt * p2;

                QPointF delta = point - curvePoint;
                double d = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (d < minDistOnCurve) {
                    minDistOnCurve = d;
                    closestOnCurve = curvePoint;
                }
            }

            segmentClosestPoint = closestOnCurve;
            distance = minDistOnCurve;

        } else {
            // Straight segment - project onto line
            QPointF p1 = current.position;
            QPointF p2 = next.position;

            QPointF segment = p2 - p1;
            QPointF toPoint = point - p1;

            double segmentLength = segment.x() * segment.x() + segment.y() * segment.y();

            if (segmentLength < 0.0001) {
                continue;  // Degenerate segment
            }

            double t = (toPoint.x() * segment.x() + toPoint.y() * segment.y()) / segmentLength;
            t = qBound(0.0, t, 1.0);

            QPointF projected = p1 + segment * t;
            QPointF delta = point - projected;
            distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

            segmentClosestPoint = projected;
        }

        if (distance < minDistance) {
            minDistance = distance;
            closestSegment = i;
            bestPoint = segmentClosestPoint;
        }
    }

    if (closestPoint) {
        *closestPoint = bestPoint;
    }

    return closestSegment;
}

} // namespace Geometry
} // namespace PatternCAD
