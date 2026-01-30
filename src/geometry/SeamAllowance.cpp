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
#include <cmath>

namespace PatternCAD {

using namespace Clipper2Lib;

SeamAllowance::SeamAllowance(QObject* parent)
    : QObject(parent)
    , m_sourcePolyline(nullptr)
    , m_cornerType(CornerType::Miter)
    , m_enabled(false)
    , m_width(10.0)
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

// Helper: check if vertex v is strictly between start and end on circular contour of size n
// Returns true if v is in the open range (start, end) going forward
static bool isVertexBetween(int v, int start, int end, int n) {
    if (start == end) return false;  // Empty range
    if (start < end) {
        return v > start && v < end;
    } else {
        // Wraps around: (start, n-1] or [0, end)
        return v > start || v < end;
    }
}

// Helper: check if vertex v is in range [start, end] (inclusive) on circular contour
static bool isVertexInRange(int v, int start, int end, int n) {
    if (start == end) return v == start;
    if (start < end) {
        return v >= start && v <= end;
    } else {
        // Wraps around
        return v >= start || v <= end;
    }
}

// New multi-range API
void SeamAllowance::addRange(int startVertexIndex, int endVertexIndex, double width)
{
    if (!m_sourcePolyline) {
        // No polyline, just add directly
        if (width > 0) {
            SeamRange range;
            range.startVertexIndex = startVertexIndex;
            range.endVertexIndex = endVertexIndex;
            range.width = width;
            range.isFullContour = false;
            m_ranges.append(range);
        }
        m_enabled = !m_ranges.isEmpty();
        emit changed();
        return;
    }
    
    int n = m_sourcePolyline->vertices().size();
    QVector<SeamRange> newRanges;
    
    for (const auto& existingRange : m_ranges) {
        if (existingRange.isFullContour) {
            // FullContour + partial range = complementary partial range
            // If we're adding [start, end], the complement is [end, start]
            SeamRange complement;
            complement.startVertexIndex = endVertexIndex;
            complement.endVertexIndex = startVertexIndex;
            complement.width = existingRange.width;
            complement.isFullContour = false;
            newRanges.append(complement);
            continue;
        }
        
        int exStart = existingRange.startVertexIndex;
        int exEnd = existingRange.endVertexIndex;
        
        // Check if new range [start, end] overlaps with existing [exStart, exEnd]
        bool newStartInEx = isVertexInRange(startVertexIndex, exStart, exEnd, n);
        bool newEndInEx = isVertexInRange(endVertexIndex, exStart, exEnd, n);
        bool exStartInNew = isVertexInRange(exStart, startVertexIndex, endVertexIndex, n);
        bool exEndInNew = isVertexInRange(exEnd, startVertexIndex, endVertexIndex, n);
        
        if (exStartInNew && exEndInNew) {
            // Existing range is completely contained in new range - remove it
            continue;
        }
        
        if (!newStartInEx && !newEndInEx && !exStartInNew && !exEndInNew) {
            // No overlap - keep existing range as is
            newRanges.append(existingRange);
            continue;
        }
        
        // Partial overlap - need to split or trim existing range
        if (newStartInEx && newEndInEx) {
            // New range is entirely within existing range - split into two parts
            // Part 1: [exStart, start]
            if (exStart != startVertexIndex) {
                SeamRange part1;
                part1.startVertexIndex = exStart;
                part1.endVertexIndex = startVertexIndex;
                part1.width = existingRange.width;
                part1.isFullContour = false;
                newRanges.append(part1);
            }
            // Part 2: [end, exEnd]
            if (endVertexIndex != exEnd) {
                SeamRange part2;
                part2.startVertexIndex = endVertexIndex;
                part2.endVertexIndex = exEnd;
                part2.width = existingRange.width;
                part2.isFullContour = false;
                newRanges.append(part2);
            }
        } else if (newStartInEx) {
            // New range starts inside existing - trim existing to [exStart, start]
            if (exStart != startVertexIndex) {
                SeamRange trimmed;
                trimmed.startVertexIndex = exStart;
                trimmed.endVertexIndex = startVertexIndex;
                trimmed.width = existingRange.width;
                trimmed.isFullContour = false;
                newRanges.append(trimmed);
            }
        } else if (newEndInEx) {
            // New range ends inside existing - trim existing to [end, exEnd]
            if (endVertexIndex != exEnd) {
                SeamRange trimmed;
                trimmed.startVertexIndex = endVertexIndex;
                trimmed.endVertexIndex = exEnd;
                trimmed.width = existingRange.width;
                trimmed.isFullContour = false;
                newRanges.append(trimmed);
            }
        } else {
            // exStart or exEnd is in new range but not both - complex overlap
            // Keep existing for now (shouldn't happen often)
            newRanges.append(existingRange);
        }
    }
    m_ranges = newRanges;
    
    // Add the new range only if width > 0
    if (width > 0) {
        SeamRange range;
        range.startVertexIndex = startVertexIndex;
        range.endVertexIndex = endVertexIndex;
        range.width = width;
        range.isFullContour = false;
        m_ranges.append(range);
    }
    
    m_enabled = !m_ranges.isEmpty();
    emit changed();
}

void SeamAllowance::addFullContour(double width)
{
    SeamRange range;
    range.isFullContour = true;
    range.width = width;
    m_ranges.append(range);
    m_enabled = true;
    emit changed();
}

void SeamAllowance::removeRange(int index)
{
    if (index >= 0 && index < m_ranges.size()) {
        m_ranges.remove(index);
        if (m_ranges.isEmpty()) {
            m_enabled = false;
        }
        emit changed();
    }
}

void SeamAllowance::clearRanges()
{
    m_ranges.clear();
    m_enabled = false;
    emit changed();
}

// Legacy single-range API (for compatibility)
void SeamAllowance::setRange(int startVertexIndex, int endVertexIndex)
{
    // For legacy API, add a new range with current width
    addRange(startVertexIndex, endVertexIndex, m_width);
}

void SeamAllowance::setFullContour(bool full)
{
    if (full) {
        addFullContour(m_width);
    }
}

bool SeamAllowance::isFullContour() const
{
    // Legacy: check if any range is full contour
    for (const auto& range : m_ranges) {
        if (range.isFullContour) return true;
    }
    return false;
}

int SeamAllowance::startVertexIndex() const
{
    // Legacy: return first range's start
    if (!m_ranges.isEmpty()) {
        return m_ranges.first().startVertexIndex;
    }
    return -1;
}

int SeamAllowance::endVertexIndex() const
{
    // Legacy: return first range's end
    if (!m_ranges.isEmpty()) {
        return m_ranges.first().endVertexIndex;
    }
    return -1;
}

bool SeamAllowance::isEdgeInRange(int edgeIndex) const
{
    if (!m_sourcePolyline) return false;
    
    int vertexCount = m_sourcePolyline->vertexCount();
    if (vertexCount == 0) return false;
    
    // Check if edge is in ANY of the ranges
    for (const auto& range : m_ranges) {
        if (range.isFullContour) return true;
        
        if (range.startVertexIndex < 0 || range.endVertexIndex < 0) continue;
        
        if (range.startVertexIndex == range.endVertexIndex) {
            return true;
        }
        
        int current = range.startVertexIndex;
        while (current != range.endVertexIndex) {
            if (edgeIndex == current) {
                return true;
            }
            current = (current + 1) % vertexCount;
        }
    }
    
    return false;
}

void SeamAllowance::clearRange()
{
    clearRanges();
}

// Main computation - returns all offset polygons
QVector<QVector<QPointF>> SeamAllowance::computeAllOffsets() const
{
    QVector<QVector<QPointF>> allOffsets;
    
    if (!m_sourcePolyline || !m_enabled) {
        return allOffsets;
    }
    
    for (const auto& range : m_ranges) {
        QVector<QPointF> offset = computeRangeOffset(range);
        if (!offset.isEmpty()) {
            allOffsets.append(offset);
        }
    }
    
    return allOffsets;
}

// Legacy single offset (returns first range only)
QVector<QPointF> SeamAllowance::computeOffset() const
{
    if (!m_sourcePolyline || !m_enabled || m_ranges.isEmpty()) {
        return QVector<QPointF>();
    }
    
    // Return first range for backwards compatibility
    return computeRangeOffset(m_ranges.first());
}

// Compute offset for a single range
QVector<QPointF> SeamAllowance::computeRangeOffset(const SeamRange& range) const
{
    if (!m_sourcePolyline || range.width <= 0.0 || !range.isValid()) {
        return QVector<QPointF>();
    }

    QVector<Geometry::PolylineVertex> vertices = m_sourcePolyline->vertices();
    if (vertices.size() < 3) {
        return QVector<QPointF>();
    }

    int n = vertices.size();

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
            joinType = JoinType::Square;
            break;
        default:
            joinType = JoinType::Miter;
            break;
    }

    if (range.isFullContour) {
        // Full contour: offset entire polygon
        PathD path;
        
        for (int i = 0; i < n; ++i) {
            int nextIdx = (i + 1) % n;
            const Geometry::PolylineVertex& current = vertices[i];
            const Geometry::PolylineVertex& next = vertices[nextIdx];
            
            path.push_back(PointD(current.position.x(), current.position.y()));
            
            bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                              (next.type == Geometry::VertexType::Smooth);
            
            if (needsCurve) {
                addCurvePoints(path, current, next, i, vertices);
            }
        }
        
        PathsD solution = InflatePaths({path}, range.width, joinType, EndType::Polygon);
        
        QVector<QPointF> result;
        if (!solution.empty()) {
            const PathD& offsetPath = solution[0];
            for (const PointD& pt : offsetPath) {
                result.append(QPointF(pt.x, pt.y));
            }
        }
        return result;
    } else {
        // Partial range: Direct manual strip construction
        // Endpoints: simple perpendicular offset (gives perpendicular termination)
        // Interior points: proper miter calculation (bisector, length = width/cos(halfAngle))
        
        // Compute polygon signed area to determine winding direction (outside direction)
        double signedArea = 0.0;
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            signedArea += vertices[i].position.x() * vertices[j].position.y();
            signedArea -= vertices[j].position.x() * vertices[i].position.y();
        }
        // outsideSign determines which perpendicular direction is "outside"
        double outsideSign = (signedArea > 0) ? -1.0 : 1.0;
        
        // Build range points including curve samples
        QVector<QPointF> rangePoints;
        int idx = range.startVertexIndex;
        while (idx != range.endVertexIndex) {
            int nextIdx = (idx + 1) % n;
            const Geometry::PolylineVertex& v1 = vertices[idx];
            const Geometry::PolylineVertex& v2 = vertices[nextIdx];
            
            rangePoints.append(v1.position);
            
            bool needsCurve = (v1.type == Geometry::VertexType::Smooth) ||
                              (v2.type == Geometry::VertexType::Smooth);
            if (needsCurve) {
                PathD curvePath;
                addCurvePoints(curvePath, v1, v2, idx, vertices);
                for (const auto& pt : curvePath) {
                    rangePoints.append(QPointF(pt.x, pt.y));
                }
            }
            
            idx = nextIdx;
        }
        rangePoints.append(vertices[range.endVertexIndex].position);
        
        int numPts = rangePoints.size();
        if (numPts < 2) {
            return QVector<QPointF>();
        }
        
        // Compute offset points with proper miter at interior corners
        QVector<QPointF> offsetPoints;
        
        for (int i = 0; i < numPts; ++i) {
            QPointF p = rangePoints[i];
            QPointF offsetPt;
            
            if (i == 0) {
                // FIRST point: simple perpendicular to first edge
                // This gives a clean perpendicular termination at start
                QPointF edge = rangePoints[1] - p;
                double len = std::sqrt(edge.x() * edge.x() + edge.y() * edge.y());
                if (len > 0.0001) edge /= len;
                
                // Normal pointing outside (perpendicular to edge, rotated by outsideSign)
                QPointF normal(-edge.y() * outsideSign, edge.x() * outsideSign);
                offsetPt = p + normal * range.width;
                
            } else if (i == numPts - 1) {
                // LAST point: simple perpendicular to last edge
                // This gives a clean perpendicular termination at end
                QPointF edge = p - rangePoints[numPts - 2];
                double len = std::sqrt(edge.x() * edge.x() + edge.y() * edge.y());
                if (len > 0.0001) edge /= len;
                
                QPointF normal(-edge.y() * outsideSign, edge.x() * outsideSign);
                offsetPt = p + normal * range.width;
                
            } else {
                // INTERIOR point: miter calculation
                // Get directions of incoming and outgoing edges
                QPointF edgeIn = p - rangePoints[i - 1];
                QPointF edgeOut = rangePoints[i + 1] - p;
                double lenIn = std::sqrt(edgeIn.x() * edgeIn.x() + edgeIn.y() * edgeIn.y());
                double lenOut = std::sqrt(edgeOut.x() * edgeOut.x() + edgeOut.y() * edgeOut.y());
                if (lenIn > 0.0001) edgeIn /= lenIn;
                if (lenOut > 0.0001) edgeOut /= lenOut;
                
                // Normals of each edge (pointing outside)
                QPointF normIn(-edgeIn.y() * outsideSign, edgeIn.x() * outsideSign);
                QPointF normOut(-edgeOut.y() * outsideSign, edgeOut.x() * outsideSign);
                
                // Bisector = average of the two normals (normalized)
                QPointF bisector(normIn.x() + normOut.x(), normIn.y() + normOut.y());
                double bisLen = std::sqrt(bisector.x() * bisector.x() + bisector.y() * bisector.y());
                
                if (bisLen > 0.0001) {
                    bisector /= bisLen;
                    
                    // cos(halfAngle) = dot(normIn, bisector)
                    double cosHalfAngle = normIn.x() * bisector.x() + normIn.y() * bisector.y();
                    
                    // Prevent extreme miters for very sharp angles
                    if (std::abs(cosHalfAngle) < 0.1) {
                        cosHalfAngle = (cosHalfAngle > 0) ? 0.1 : -0.1;
                    }
                    
                    // Miter length = width / cos(halfAngle)
                    double miterLen = range.width / cosHalfAngle;
                    
                    // Cap miter length to prevent extremely long spikes
                    double maxMiter = range.width * 5.0;
                    if (std::abs(miterLen) > maxMiter) {
                        miterLen = (miterLen > 0) ? maxMiter : -maxMiter;
                    }
                    
                    offsetPt = p + bisector * miterLen;
                } else {
                    // Degenerate case (parallel edges): use simple perpendicular
                    offsetPt = p + normIn * range.width;
                }
            }
            
            offsetPoints.append(offsetPt);
        }
        
        // Build the closed strip polygon:
        // Original contour (start to end) + Offset contour (end to start, reversed)
        QVector<QPointF> result;
        
        // Add original contour points
        for (const QPointF& pt : rangePoints) {
            result.append(pt);
        }
        
        // Add offset contour points in reverse order
        for (int i = offsetPoints.size() - 1; i >= 0; --i) {
            result.append(offsetPoints[i]);
        }
        
        return result;
    }
}

void SeamAllowance::addCurvePoints(PathD& path, 
                                    const Geometry::PolylineVertex& current,
                                    const Geometry::PolylineVertex& next,
                                    int currentIdx,
                                    const QVector<Geometry::PolylineVertex>& vertices) const
{
    int n = vertices.size();
    QPointF p1 = current.position;
    QPointF p2 = next.position;
    
    QPointF segment = p2 - p1;
    double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
    double controlDistance = dist / 3.0;
    
    QPointF c1, c2;
    
    if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
        c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
    } else if (current.type == Geometry::VertexType::Smooth) {
        int prevIdx = (currentIdx - 1 + n) % n;
        QPointF p0 = vertices[prevIdx].position;
        c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
    } else {
        c1 = p1 + (p2 - p1) * 0.01;
    }
    
    if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
        c2 = p2 - next.tangent * controlDistance * next.incomingTension;
    } else if (next.type == Geometry::VertexType::Smooth) {
        int nextNextIdx = (currentIdx + 2) % n;
        QPointF p3 = vertices[nextNextIdx].position;
        c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
    } else {
        c2 = p2 - (p2 - p1) * 0.01;
    }
    
    const int segments = 20;
    for (int j = 1; j < segments; ++j) {
        double t = static_cast<double>(j) / segments;
        double t2 = t * t;
        double t3 = t2 * t;
        double mt = 1.0 - t;
        double mt2 = mt * mt;
        double mt3 = mt2 * mt;
        
        QPointF point = p1 * mt3 + c1 * 3.0 * mt2 * t + c2 * 3.0 * mt * t2 + p2 * t3;
        path.push_back(PointD(point.x(), point.y()));
    }
}

void SeamAllowance::render(QPainter* painter, const QColor& color) const
{
    if (!m_enabled || m_ranges.isEmpty()) {
        return;
    }

    painter->save();

    // Draw with dashed line
    QPen pen(color);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw ALL offset paths
    QVector<QVector<QPointF>> allOffsets = computeAllOffsets();
    
    for (const auto& offset : allOffsets) {
        if (offset.isEmpty()) continue;
        
        QPainterPath path;
        path.moveTo(offset.first());
        for (int i = 1; i < offset.size(); ++i) {
            path.lineTo(offset[i]);
        }
        path.closeSubpath();
        
        painter->drawPath(path);
    }

    painter->restore();
}

} // namespace PatternCAD
