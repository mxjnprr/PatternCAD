/**
 * AddPointOnContourTool.h
 *
 * Tool for adding points on existing contours/lines
 */

#ifndef PATTERNCAD_TOOLS_ADDPOINTONCONTOURTOOL_H
#define PATTERNCAD_TOOLS_ADDPOINTONCONTOURTOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Line;
    class Polyline;
}

namespace Tools {

/**
 * AddPointOnContourTool allows adding points to existing lines and polylines
 * - Click on a line to add a point at that location
 * - If it's a simple Line, converts it to a Polyline
 * - If it's a Polyline, inserts a new vertex at the click location
 */
class AddPointOnContourTool : public Tool
{
    Q_OBJECT

public:
    explicit AddPointOnContourTool(QObject* parent = nullptr);
    ~AddPointOnContourTool() override;

    QString name() const override;
    QString description() const override;
    QCursor cursor() const override;

    void activate() override;
    void reset() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawOverlay(QPainter* painter) override;

private:
    enum class EditMode {
        AddingPoint,      // Adding new point to contour
        DraggingVertex    // Dragging existing vertex
    };

    Geometry::GeometryObject* findLineOrPolylineAt(const QPointF& point);
    void addPointToLine(Geometry::Line* line, const QPointF& point);
    void addPointToPolyline(Geometry::Polyline* polyline, const QPointF& point);

    // Vertex manipulation
    int findVertexAt(const QPointF& point, Geometry::GeometryObject** outObject) const;
    void drawVertexHandles(QPainter* painter, Geometry::GeometryObject* obj) const;
    void toggleVertexType();
    void deleteSelectedVertex();

    Geometry::GeometryObject* m_hoveredObject;
    QPointF m_currentPoint;
    QPointF m_projectedPoint;
    bool m_hasProjection;

    EditMode m_mode;
    Geometry::GeometryObject* m_selectedVertexObject;
    int m_selectedVertexIndex;
    int m_hoveredVertexIndex;
    QPointF m_lastPoint;
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_TOOLS_ADDPOINTONCONTOURTOOL_H
