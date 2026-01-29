/**
 * PolylineTool.h
 *
 * Tool for drawing closed polylines with sharp and smooth vertices
 */

#ifndef PATTERNCAD_POLYLINETOOL_H
#define PATTERNCAD_POLYLINETOOL_H

#include "Tool.h"
#include "geometry/Polyline.h"
#include <QPointF>
#include <QVector>

namespace PatternCAD {
namespace Tools {

/**
 * PolylineTool provides polyline drawing functionality:
 * - Click to add sharp vertex
 * - Shift+Click to add smooth vertex (curve)
 * - Double-click or Enter to finish and close polyline
 * - Escape to cancel
 * - Visual preview of current polyline
 */
class PolylineTool : public Tool
{
    Q_OBJECT

public:
    explicit PolylineTool(QObject* parent = nullptr);
    ~PolylineTool() override;

    // Tool identification
    QString name() const override;
    QString description() const override;

    // Tool lifecycle
    void activate() override;
    void reset() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Drawing overlay
    void drawOverlay(QPainter* painter) override;

    // Apply dimension input (angle in degrees, 0 = use current direction)
    void applyLength(double lengthInMm, double angleDegrees = 0.0);

private:
    QVector<Geometry::PolylineVertex> m_vertices;
    QPointF m_currentPoint;
    QPointF m_pressPoint;  // Where mouse was pressed
    bool m_isDrawing;
    bool m_shiftPressed;
    bool m_isDragging;     // True if mouse moved significantly after press

    // Helper methods
    void addVertex(const QPointF& point, Geometry::VertexType type, double tension, const QPointF& tangent = QPointF());
    void finishPolyline();
    void cancelPolyline();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_POLYLINETOOL_H
