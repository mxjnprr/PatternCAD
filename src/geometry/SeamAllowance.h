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
#include <set>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Polyline;
}

/**
 * Corner type for seam allowance offsetting
 */
enum class CornerType {
    Miter,   // Sharp mitered corners
    Round,   // Rounded corners
    Bevel    // Beveled (flat) corners
};

/**
 * SeamAllowance generates offset outlines for pattern pieces.
 * Uses Clipper2 library for polygon offsetting.
 */
class SeamAllowance : public QObject
{
    Q_OBJECT

public:
    explicit SeamAllowance(QObject* parent = nullptr);
    ~SeamAllowance();

    // Configuration
    void setWidth(double width);
    double width() const { return m_width; }

    void setCornerType(CornerType type);
    CornerType cornerType() const { return m_cornerType; }

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    // Source geometry
    void setSourcePolyline(Geometry::Polyline* polyline);
    Geometry::Polyline* sourcePolyline() const { return m_sourcePolyline; }

    // Edge exclusion (for future use)
    void excludeEdge(int edgeIndex);
    void includeEdge(int edgeIndex);
    bool isEdgeExcluded(int edgeIndex) const;
    void clearExcludedEdges();

    // Computation
    QVector<QPointF> computeOffset() const;

    // Rendering
    void render(QPainter* painter, const QColor& color = Qt::red) const;

signals:
    void changed();

private:
    Geometry::Polyline* m_sourcePolyline;
    double m_width;
    CornerType m_cornerType;
    bool m_enabled;
    std::set<int> m_excludedEdges;

    // Helper to convert Clipper2 types
    void convertToClipper(const QVector<QPointF>& points, void* clipperPath) const;
    QVector<QPointF> convertFromClipper(const void* clipperPath) const;
};

} // namespace PatternCAD

#endif // PATTERNCAD_SEAMALLOWANCE_H
