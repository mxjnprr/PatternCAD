/**
 * Notch.cpp
 *
 * Implementation of Notch class for pattern assembly markers
 */

#include "Notch.h"
#include "Polyline.h"
#include <QUuid>
#include <QtMath>
#include <cmath>

namespace PatternCAD {

namespace {
    // Calculate point on a line segment at parameter t (0-1)
    QPointF pointOnSegment(const QPointF& p1, const QPointF& p2, double t) {
        return p1 + (p2 - p1) * t;
    }

    // Normalize a vector
    QPointF normalize(const QPointF& v) {
        double len = std::sqrt(v.x() * v.x() + v.y() * v.y());
        if (len < 1e-10) return QPointF(1, 0);
        return QPointF(v.x() / len, v.y() / len);
    }

    // Get perpendicular vector (rotate 90 degrees CCW)
    QPointF perpendicular(const QPointF& v) {
        return QPointF(-v.y(), v.x());
    }
}

Notch::Notch(QObject* parent)
    : QObject(parent)
    , m_polyline(nullptr)
    , m_segmentIndex(0)
    , m_position(0.5)
    , m_style(NotchStyle::VNotch)
    , m_depth(5.0)
{
    m_id = generateId();
}

Notch::Notch(Geometry::Polyline* polyline, int segmentIndex, double position,
             NotchStyle style, double depth, QObject* parent)
    : QObject(parent)
    , m_polyline(polyline)
    , m_segmentIndex(segmentIndex)
    , m_position(qBound(0.0, position, 1.0))
    , m_style(style)
    , m_depth(depth)
{
    m_id = generateId();
}

Notch::~Notch()
{
}

QString Notch::generateId() const
{
    return "notch_" + QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

void Notch::setPolyline(Geometry::Polyline* polyline)
{
    if (m_polyline != polyline) {
        m_polyline = polyline;
        emit changed();
    }
}

void Notch::setSegmentIndex(int index)
{
    if (m_segmentIndex != index) {
        m_segmentIndex = index;
        emit changed();
    }
}

void Notch::setPosition(double pos)
{
    pos = qBound(0.0, pos, 1.0);
    if (!qFuzzyCompare(m_position, pos)) {
        m_position = pos;
        emit changed();
    }
}

void Notch::setStyle(NotchStyle style)
{
    if (m_style != style) {
        m_style = style;
        emit changed();
    }
}

void Notch::setDepth(double depth)
{
    depth = qMax(0.0, depth);
    if (!qFuzzyCompare(m_depth, depth)) {
        m_depth = depth;
        emit changed();
    }
}

QPointF Notch::getLocation() const
{
    if (!m_polyline) {
        return QPointF();
    }

    QVector<Geometry::PolylineVertex> vertices = m_polyline->vertices();
    int n = vertices.size();
    
    if (n < 2 || m_segmentIndex < 0 || m_segmentIndex >= n) {
        return QPointF();
    }

    int nextIdx = (m_segmentIndex + 1) % n;
    
    // For now, use linear interpolation between segment endpoints
    // TODO: Handle curved segments with proper Bezier evaluation
    QPointF p1 = vertices[m_segmentIndex].position;
    QPointF p2 = vertices[nextIdx].position;

    return pointOnSegment(p1, p2, m_position);
}

QPointF Notch::getNormal() const
{
    if (!m_polyline) {
        return QPointF(0, -1);  // Default upward
    }

    QVector<Geometry::PolylineVertex> vertices = m_polyline->vertices();
    int n = vertices.size();
    
    if (n < 2 || m_segmentIndex < 0 || m_segmentIndex >= n) {
        return QPointF(0, -1);
    }

    int nextIdx = (m_segmentIndex + 1) % n;
    
    QPointF p1 = vertices[m_segmentIndex].position;
    QPointF p2 = vertices[nextIdx].position;

    // Direction along the edge
    QPointF edgeDir = normalize(p2 - p1);
    
    // Normal is perpendicular to edge, pointing outward
    // For a closed polyline with CCW vertex order, outward normal is to the left
    QPointF normal = perpendicular(edgeDir);
    
    return normal;
}

void Notch::render(QPainter* painter, const QColor& color) const
{
    if (!m_polyline) {
        return;
    }

    QPointF loc = getLocation();
    QPointF normal = getNormal();

    painter->save();
    
    QPen pen(color);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    switch (m_style) {
        case NotchStyle::VNotch:
            renderVNotch(painter, loc, normal);
            break;
        case NotchStyle::Slit:
            renderSlit(painter, loc, normal);
            break;
        case NotchStyle::Dot:
            renderDot(painter, loc);
            break;
    }

    painter->restore();
}

void Notch::renderVNotch(QPainter* painter, const QPointF& loc, const QPointF& normal) const
{
    // V-notch: Two lines forming a V shape pointing inward
    double halfWidth = m_depth * 0.5;  // Width at base of V
    
    // Tangent direction (perpendicular to normal)
    QPointF tangent(-normal.y(), normal.x());
    
    // V points inward (opposite to normal)
    QPointF tip = loc - normal * m_depth;
    QPointF left = loc + tangent * halfWidth;
    QPointF right = loc - tangent * halfWidth;

    painter->drawLine(left, tip);
    painter->drawLine(right, tip);
}

void Notch::renderSlit(QPainter* painter, const QPointF& loc, const QPointF& normal) const
{
    // Slit: Single straight line perpendicular to edge, pointing inward
    QPointF inner = loc - normal * m_depth;
    
    painter->drawLine(loc, inner);
}

void Notch::renderDot(QPainter* painter, const QPointF& loc) const
{
    // Dot: Small filled circle
    double radius = m_depth * 0.3;  // Dot size relative to depth
    if (radius < 1.5) radius = 1.5;
    
    painter->setBrush(painter->pen().color());
    painter->drawEllipse(loc, radius, radius);
}

Notch* Notch::clone(Geometry::Polyline* newPolyline) const
{
    Notch* copy = new Notch();
    copy->m_polyline = newPolyline ? newPolyline : m_polyline;
    copy->m_segmentIndex = m_segmentIndex;
    copy->m_position = m_position;
    copy->m_style = m_style;
    copy->m_depth = m_depth;
    // Note: ID is regenerated for the clone
    return copy;
}

QJsonObject Notch::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["segmentIndex"] = m_segmentIndex;
    json["position"] = m_position;
    json["style"] = static_cast<int>(m_style);
    json["depth"] = m_depth;
    return json;
}

Notch* Notch::fromJson(const QJsonObject& json, Geometry::Polyline* polyline)
{
    Notch* notch = new Notch();
    notch->m_polyline = polyline;
    
    if (json.contains("id")) {
        notch->m_id = json["id"].toString();
    }
    if (json.contains("segmentIndex")) {
        notch->m_segmentIndex = json["segmentIndex"].toInt();
    }
    if (json.contains("position")) {
        notch->m_position = json["position"].toDouble();
    }
    if (json.contains("style")) {
        notch->m_style = static_cast<NotchStyle>(json["style"].toInt());
    }
    if (json.contains("depth")) {
        notch->m_depth = json["depth"].toDouble();
    }
    
    return notch;
}

} // namespace PatternCAD
