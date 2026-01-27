/**
 * SelectTool.h
 *
 * Selection tool for selecting and manipulating objects
 */

#ifndef PATTERNCAD_SELECTTOOL_H
#define PATTERNCAD_SELECTTOOL_H

#include "Tool.h"
#include <QPointF>
#include <QRectF>
#include <QList>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
}

namespace Tools {

/**
 * SelectTool provides selection functionality:
 * - Click to select single objects
 * - Click-drag for rectangular selection
 * - Ctrl+Click for multi-selection
 * - Move selected objects by dragging
 * - Display selection handles
 */
class SelectTool : public Tool
{
    Q_OBJECT

public:
    explicit SelectTool(QObject* parent = nullptr);
    ~SelectTool();

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
    void keyPressEvent(QKeyEvent* event) override;

    // Drawing overlay
    void drawOverlay(QPainter* painter) override;

private:
    enum class SelectMode {
        None,
        Selecting,  // Drawing selection rectangle
        Moving      // Moving selected objects
    };

    SelectMode m_mode;
    QPointF m_startPoint;
    QPointF m_currentPoint;
    QPointF m_lastPoint;
    QRectF m_selectionRect;
    bool m_multiSelect;

    // Helper methods
    void selectObjectAt(const QPointF& point, bool addToSelection);
    void selectObjectsInRect(const QRectF& rect, bool addToSelection);
    void startMoving();
    void updateMove(const QPointF& delta);
    void finishMove();
    Geometry::GeometryObject* findObjectAt(const QPointF& point) const;
    QList<Geometry::GeometryObject*> findObjectsInRect(const QRectF& rect) const;
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_SELECTTOOL_H
