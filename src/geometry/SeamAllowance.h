/**
 * SeamAllowance.h
 *
 * Automatic seam allowance generation for pattern pieces
 */

#ifndef PATTERNCAD_SEAMALLOWANCE_H
#define PATTERNCAD_SEAMALLOWANCE_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QPainter>
#include "Polyline.h"
#include <clipper2/clipper.h>

namespace PatternCAD {

/**
 * Corner type for seam allowance offsetting
 */
enum class CornerType {
    Miter,   // Sharp mitered corners
    Round,   // Rounded corners
    Bevel    // Beveled (flat) corners
};

/**
 * A single seam allowance range on a polyline
 */
struct SeamRange {
    int startVertexIndex = -1;
    int endVertexIndex = -1;
    double width = 10.0;
    bool isFullContour = false;
    
    bool isValid() const { 
        return isFullContour || (startVertexIndex >= 0 && endVertexIndex >= 0); 
    }
};

/**
 * SeamAllowance generates offset outlines for pattern pieces.
 * Uses Clipper2 library for polygon offsetting.
 * 
 * Supports MULTIPLE range-based selections on a single piece:
 * - addRange(start, end, width): adds a seam allowance from startVertex to endVertex
 * - addFullContour(width): adds seam allowance to entire contour
 * - removeRange(index): removes a specific range
 * - clearRanges(): removes all ranges
 */
class SeamAllowance : public QObject
{
    Q_OBJECT

public:
    explicit SeamAllowance(QObject* parent = nullptr);
    ~SeamAllowance();

    // Configuration
    void setCornerType(CornerType type);
    CornerType cornerType() const { return m_cornerType; }

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    // Source geometry
    void setSourcePolyline(Geometry::Polyline* polyline);
    Geometry::Polyline* sourcePolyline() const { return m_sourcePolyline; }

    // Multiple ranges support
    void addRange(int startVertexIndex, int endVertexIndex, double width);
    void addFullContour(double width);
    void removeRange(int index);
    void clearRanges();
    int rangeCount() const { return m_ranges.size(); }
    const SeamRange& range(int index) const { return m_ranges[index]; }
    
    // Legacy single-range API (deprecated, for compatibility)
    void setRange(int startVertexIndex, int endVertexIndex);
    void setFullContour(bool full);
    void setWidth(double width);
    double width() const { return m_width; }
    bool isFullContour() const;
    int startVertexIndex() const;
    int endVertexIndex() const;
    bool isEdgeInRange(int edgeIndex) const;
    void clearRange();

    // Computation - returns all offset polygons
    QVector<QVector<QPointF>> computeAllOffsets() const;
    
    // Legacy single offset (returns first range only)
    QVector<QPointF> computeOffset() const;

    // Rendering
    void render(QPainter* painter, const QColor& color = Qt::red) const;

signals:
    void changed();

private:
    Geometry::Polyline* m_sourcePolyline;
    CornerType m_cornerType;
    bool m_enabled;
    
    // Multiple ranges
    QVector<SeamRange> m_ranges;
    
    // Legacy single range (for compatibility)
    double m_width;
    
    // Helper methods
    QVector<QPointF> computeRangeOffset(const SeamRange& range) const;
    void addCurvePoints(Clipper2Lib::PathD& path, 
                        const Geometry::PolylineVertex& current,
                        const Geometry::PolylineVertex& next,
                        int currentIdx,
                        const QVector<Geometry::PolylineVertex>& vertices) const;
};

} // namespace PatternCAD

#endif // PATTERNCAD_SEAMALLOWANCE_H
