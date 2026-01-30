/**
 * SeamAllowanceTool.h
 *
 * Tool for applying and configuring seam allowances on pattern pieces
 */

#ifndef PATTERNCAD_SEAMALLOWANCETOOL_H
#define PATTERNCAD_SEAMALLOWANCETOOL_H

#include "Tool.h"
#include "geometry/SeamAllowance.h"
#include <QPointF>
#include <QElapsedTimer>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Polyline;
}

namespace Tools {

/**
 * SeamAllowanceTool allows users to:
 * - Select pattern pieces (polylines)
 * - Click on vertices to define seam allowance range
 * - Double-click on a vertex for full contour
 * - Configure seam allowance width (default 10mm)
 * - Keyboard shortcut: 'S'
 * 
 * Workflow:
 * 1. Click on a polyline to select it
 * 2. Click on a vertex (start point)
 * 3. Click on another vertex (end point) - seam applies clockwise from start to end
 *    OR double-click on same vertex - seam applies to full contour
 */
class SeamAllowanceTool : public Tool
{
    Q_OBJECT

public:
    explicit SeamAllowanceTool(QObject* parent = nullptr);
    ~SeamAllowanceTool();

    // Tool identification
    QString name() const override;
    QString description() const override;
    QCursor cursor() const override;

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

    // Configuration
    void setWidth(double width);
    void setCornerType(CornerType type);
    double width() const { return m_width; }
    CornerType cornerType() const { return m_cornerType; }

signals:
    // Configuration changed
    void widthChanged(double width);
    void cornerTypeChanged(CornerType type);

private:
    enum class Mode {
        SelectingPolyline,     // Selecting which polyline to work on
        SelectingStartPoint,   // Selecting the first vertex
        SelectingEndPoint      // Selecting the second vertex
    };

    Mode m_mode;
    double m_width;              // Current seam allowance width (default 10mm)
    CornerType m_cornerType;     // Current corner type

    // Current selection
    Geometry::Polyline* m_selectedPolyline;  // Currently selected polyline
    int m_hoveredVertexIndex;                // Index of vertex under mouse (-1 if none)
    int m_startVertexIndex;                  // First selected vertex

    // For double-click detection
    int m_lastClickedVertex;
    QElapsedTimer m_doubleClickTimer;
    static constexpr int DOUBLE_CLICK_MS = 400;

    // Helper methods
    Geometry::Polyline* findPolylineAt(const QPointF& point) const;
    int findVertexAt(Geometry::Polyline* polyline, const QPointF& point) const;
    void selectPolyline(Geometry::Polyline* polyline);
    void applySeamAllowance(int startVertex, int endVertex);
    void applyFullContourSeamAllowance();
    void drawVertexHighlights(QPainter* painter) const;
    void drawRangePreview(QPainter* painter) const;
    void updateStatusMessage();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_SEAMALLOWANCETOOL_H
