/**
 * Canvas.h
 *
 * QGraphicsView subclass for pattern editing viewport
 */

#ifndef PATTERNCAD_CANVAS_H
#define PATTERNCAD_CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>

namespace PatternCAD {

// Forward declarations
class Document;

namespace Geometry {
    class GeometryObject;
}

namespace Tools {
    class Tool;
}

namespace UI {

class DimensionRenderer;

/**
 * Canvas provides the main editing viewport with:
 * - Pan and zoom functionality
 * - Grid display
 * - Snap-to-grid support
 * - Interactive drawing and selection
 * - Real-time tool feedback
 */
class Canvas : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    // Document
    Document* document() const;
    void setDocument(Document* document);

    // View manipulation
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomToSelection();
    void zoomToObject(Geometry::GeometryObject* object);
    void zoomToActual();  // 100% zoom
    void zoomReset();
    void setZoomLevel(double level);
    double zoomLevel() const;

    // Grid
    bool gridVisible() const;
    void setGridVisible(bool visible);
    bool snapToGrid() const;
    void setSnapToGrid(bool snap);

    // Coordinate conversion
    QPointF snapPoint(const QPointF& point) const;

    // Tool management
    void setActiveTool(Tools::Tool* tool);
    Tools::Tool* activeTool() const;

signals:
    void zoomChanged(double zoom);
    void cursorPositionChanged(const QPointF& position);
    void escapePressed();
    void toolRequested(const QString& toolName);

protected:
    // Event handlers
    bool event(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;

private:
    // Private members
    QGraphicsScene* m_scene;
    Document* m_document;
    Tools::Tool* m_activeTool;
    DimensionRenderer* m_dimensionRenderer;
    double m_zoomLevel;
    bool m_gridVisible;
    bool m_snapToGrid;
    QPointF m_lastMousePos;

    // Pan state
    bool m_isPanning;
    QPoint m_panStartPos;

    // Helper methods
    void setupScene();
    void updateGrid();
    void drawGrid(QPainter* painter, const QRectF& rect);
    void drawOriginIndicator(QPainter* painter);
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_CANVAS_H
