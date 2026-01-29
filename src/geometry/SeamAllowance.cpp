/**
 * SeamAllowance.cpp
 *
 * Implementation of SeamAllowance
 */

#include "SeamAllowance.h"
#include "Polyline.h"
#include <clipper2/clipper.h>
#include <QPainterPath>
#include <QDebug>

namespace PatternCAD {

using namespace Clipper2Lib;

SeamAllowance::SeamAllowance(QObject* parent)
    : QObject(parent)
    , m_sourcePolyline(nullptr)
    , m_width(10.0)  // Default 10mm
    , m_cornerType(CornerType::Round)
    , m_enabled(true)
{
}

SeamAllowance::~SeamAllowance()
{
}

void SeamAllowance::setWidth(double width)
{
    if (m_width != width) {
        m_width = width;
        emit changed();
    }
}

void SeamAllowance::setCornerType(CornerType type)
{
    if (m_cornerType != type) {
        m_cornerType = type;
        emit changed();
    }
}

void SeamAllowance::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit changed();
    }
}

void SeamAllowance::setSourcePolyline(Geometry::Polyline* polyline)
{
    if (m_sourcePolyline != polyline) {
        m_sourcePolyline = polyline;
        emit changed();
    }
}

void SeamAllowance::excludeEdge(int edgeIndex)
{
    m_excludedEdges.insert(edgeIndex);
    emit changed();
}

void SeamAllowance::includeEdge(int edgeIndex)
{
    m_excludedEdges.erase(edgeIndex);
    emit changed();
}

bool SeamAllowance::isEdgeExcluded(int edgeIndex) const
{
    return m_excludedEdges.find(edgeIndex) != m_excludedEdges.end();
}

void SeamAllowance::clearExcludedEdges()
{
    if (!m_excludedEdges.empty()) {
        m_excludedEdges.clear();
        emit changed();
    }
}

QVector<QPointF> SeamAllowance::computeOffset() const
{
    if (!m_sourcePolyline || !m_enabled || m_width <= 0.0) {
        return QVector<QPointF>();
    }

    // Get vertices from source polyline
    QVector<Geometry::PolylineVertex> vertices = m_sourcePolyline->vertices();
    if (vertices.size() < 3) {
        return QVector<QPointF>();
    }

    // Convert to Clipper2 path
    PathD path;
    for (const auto& vertex : vertices) {
        path.push_back(PointD(vertex.position.x(), vertex.position.y()));
    }

    // Determine join type based on corner type
    JoinType joinType;
    switch (m_cornerType) {
        case CornerType::Miter:
            joinType = JoinType::Miter;
            break;
        case CornerType::Round:
            joinType = JoinType::Round;
            break;
        case CornerType::Bevel:
            joinType = JoinType::Square;  // Clipper2 uses Square for bevel
            break;
        default:
            joinType = JoinType::Round;
            break;
    }

    // Perform offset
    PathsD solution;
    solution = InflatePaths({path}, m_width, joinType, EndType::Polygon);

    // Convert result back to QPointF
    QVector<QPointF> result;
    if (!solution.empty()) {
        const PathD& offsetPath = solution[0];
        for (const PointD& pt : offsetPath) {
            result.append(QPointF(pt.x, pt.y));
        }
    }

    return result;
}

void SeamAllowance::render(QPainter* painter, const QColor& color) const
{
    if (!m_enabled) {
        return;
    }

    QVector<QPointF> offset = computeOffset();
    if (offset.isEmpty()) {
        return;
    }

    painter->save();

    // Draw with dashed line
    QPen pen(color);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw offset path
    QPainterPath path;
    path.moveTo(offset.first());
    for (int i = 1; i < offset.size(); ++i) {
        path.lineTo(offset[i]);
    }
    path.closeSubpath();

    painter->drawPath(path);

    painter->restore();
}

} // namespace PatternCAD
