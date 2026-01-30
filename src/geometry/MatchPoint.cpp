/**
 * MatchPoint.cpp
 *
 * Implementation of MatchPoint class for pattern assembly alignment markers
 */

#include "MatchPoint.h"
#include "Polyline.h"
#include <QUuid>
#include <QFont>
#include <QJsonArray>
#include <algorithm>

namespace PatternCAD {

namespace {
    // Calculate point on a line segment at parameter t (0-1)
    QPointF pointOnSegment(const QPointF& p1, const QPointF& p2, double t) {
        return p1 + (p2 - p1) * t;
    }
}

MatchPoint::MatchPoint(QObject* parent)
    : QObject(parent)
    , m_label("A")
    , m_isOnEdge(false)
    , m_polyline(nullptr)
    , m_segmentIndex(0)
    , m_segmentPosition(0.5)
{
    m_id = generateId();
}

MatchPoint::MatchPoint(const QString& label, const QPointF& position,
                       Geometry::Polyline* polyline, QObject* parent)
    : QObject(parent)
    , m_label(label)
    , m_absolutePosition(position)
    , m_isOnEdge(false)
    , m_polyline(polyline)
    , m_segmentIndex(0)
    , m_segmentPosition(0.5)
{
    m_id = generateId();
}

MatchPoint::MatchPoint(const QString& label, Geometry::Polyline* polyline,
                       int segmentIndex, double segmentPosition, QObject* parent)
    : QObject(parent)
    , m_label(label)
    , m_isOnEdge(true)
    , m_polyline(polyline)
    , m_segmentIndex(segmentIndex)
    , m_segmentPosition(qBound(0.0, segmentPosition, 1.0))
{
    m_id = generateId();
}

MatchPoint::~MatchPoint()
{
    // Unlink from all connected points
    unlinkAll();
}

QString MatchPoint::generateId() const
{
    return "mp_" + QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

void MatchPoint::setLabel(const QString& label)
{
    if (m_label != label) {
        m_label = label;
        emit changed();
    }
}

QPointF MatchPoint::position() const
{
    if (m_isOnEdge && m_polyline) {
        return calculateEdgePosition();
    }
    return m_absolutePosition;
}

void MatchPoint::setPosition(const QPointF& pos)
{
    if (m_absolutePosition != pos) {
        m_absolutePosition = pos;
        m_isOnEdge = false;  // Switch to absolute positioning
        emit changed();
    }
}

void MatchPoint::setPolyline(Geometry::Polyline* polyline)
{
    if (m_polyline != polyline) {
        m_polyline = polyline;
        emit changed();
    }
}

void MatchPoint::setSegmentIndex(int index)
{
    if (m_segmentIndex != index) {
        m_segmentIndex = index;
        m_isOnEdge = true;
        emit changed();
    }
}

void MatchPoint::setSegmentPosition(double pos)
{
    pos = qBound(0.0, pos, 1.0);
    if (!qFuzzyCompare(m_segmentPosition, pos)) {
        m_segmentPosition = pos;
        m_isOnEdge = true;
        emit changed();
    }
}

QPointF MatchPoint::calculateEdgePosition() const
{
    if (!m_polyline) {
        return m_absolutePosition;
    }

    QVector<Geometry::PolylineVertex> vertices = m_polyline->vertices();
    int n = vertices.size();
    
    if (n < 2 || m_segmentIndex < 0 || m_segmentIndex >= n) {
        return m_absolutePosition;
    }

    int nextIdx = (m_segmentIndex + 1) % n;
    
    QPointF p1 = vertices[m_segmentIndex].position;
    QPointF p2 = vertices[nextIdx].position;

    return pointOnSegment(p1, p2, m_segmentPosition);
}

void MatchPoint::linkTo(MatchPoint* other)
{
    if (!other || other == this) {
        return;
    }
    
    // Check if already linked
    if (m_linkedPoints.contains(other)) {
        return;
    }
    
    // Create bidirectional link
    m_linkedPoints.append(other);
    other->m_linkedPoints.append(this);
    
    emit linkChanged();
    emit other->linkChanged();
}

void MatchPoint::unlinkFrom(MatchPoint* other)
{
    if (!other) {
        return;
    }
    
    // Remove from both sides
    m_linkedPoints.removeAll(other);
    other->m_linkedPoints.removeAll(this);
    
    emit linkChanged();
    emit other->linkChanged();
}

void MatchPoint::unlinkAll()
{
    // Make a copy since we're modifying the list
    QVector<MatchPoint*> links = m_linkedPoints;
    
    for (MatchPoint* other : links) {
        other->m_linkedPoints.removeAll(this);
        emit other->linkChanged();
    }
    
    m_linkedPoints.clear();
    emit linkChanged();
}

bool MatchPoint::isLinkedTo(MatchPoint* other) const
{
    return m_linkedPoints.contains(other);
}

void MatchPoint::render(QPainter* painter, const QColor& color) const
{
    QPointF pos = position();
    
    painter->save();
    
    QPen pen(color);
    pen.setWidth(1);
    painter->setPen(pen);
    
    // Draw circle with cross
    const double radius = 4.0;
    
    // Circle
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(pos, radius, radius);
    
    // Cross inside circle
    painter->drawLine(pos - QPointF(radius * 0.7, 0), pos + QPointF(radius * 0.7, 0));
    painter->drawLine(pos - QPointF(0, radius * 0.7), pos + QPointF(0, radius * 0.7));
    
    // Draw label
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(9);
    painter->setFont(font);
    
    QPointF labelOffset(radius + 3, -radius - 2);
    painter->drawText(pos + labelOffset, m_label);
    
    painter->restore();
}

void MatchPoint::renderLinks(QPainter* painter, const QColor& color) const
{
    if (m_linkedPoints.isEmpty()) {
        return;
    }
    
    QPointF myPos = position();
    
    painter->save();
    
    QPen pen(color);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    painter->setPen(pen);
    
    for (const MatchPoint* other : m_linkedPoints) {
        // Only draw if this point's ID is less than the other's to avoid drawing twice
        if (m_id < other->m_id) {
            painter->drawLine(myPos, other->position());
        }
    }
    
    painter->restore();
}

MatchPoint* MatchPoint::clone(Geometry::Polyline* newPolyline) const
{
    MatchPoint* copy = new MatchPoint();
    copy->m_label = m_label;
    copy->m_absolutePosition = m_absolutePosition;
    copy->m_isOnEdge = m_isOnEdge;
    copy->m_polyline = newPolyline ? newPolyline : m_polyline;
    copy->m_segmentIndex = m_segmentIndex;
    copy->m_segmentPosition = m_segmentPosition;
    // Note: Links are NOT copied - they need to be re-established manually
    // Note: ID is regenerated for the clone
    return copy;
}

QJsonObject MatchPoint::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["label"] = m_label;
    json["absoluteX"] = m_absolutePosition.x();
    json["absoluteY"] = m_absolutePosition.y();
    json["isOnEdge"] = m_isOnEdge;
    json["segmentIndex"] = m_segmentIndex;
    json["segmentPosition"] = m_segmentPosition;
    
    // Store linked point IDs
    QJsonArray linkedIds;
    for (const MatchPoint* linked : m_linkedPoints) {
        linkedIds.append(linked->id());
    }
    if (!linkedIds.isEmpty()) {
        json["linkedPointIds"] = linkedIds;
    }
    
    return json;
}

MatchPoint* MatchPoint::fromJson(const QJsonObject& json, Geometry::Polyline* polyline)
{
    MatchPoint* mp = new MatchPoint();
    mp->m_polyline = polyline;
    
    if (json.contains("id")) {
        mp->m_id = json["id"].toString();
    }
    if (json.contains("label")) {
        mp->m_label = json["label"].toString();
    }
    if (json.contains("absoluteX") && json.contains("absoluteY")) {
        mp->m_absolutePosition = QPointF(
            json["absoluteX"].toDouble(),
            json["absoluteY"].toDouble()
        );
    }
    if (json.contains("isOnEdge")) {
        mp->m_isOnEdge = json["isOnEdge"].toBool();
    }
    if (json.contains("segmentIndex")) {
        mp->m_segmentIndex = json["segmentIndex"].toInt();
    }
    if (json.contains("segmentPosition")) {
        mp->m_segmentPosition = json["segmentPosition"].toDouble();
    }
    
    // Note: Links are restored separately after all match points are loaded
    
    return mp;
}

} // namespace PatternCAD
