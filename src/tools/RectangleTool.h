/**
 * RectangleTool.h
 *
 * Tool for drawing rectangles
 */

#ifndef PATTERNCAD_RECTANGLETOOL_H
#define PATTERNCAD_RECTANGLETOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {
namespace Tools {

/**
 * RectangleTool provides rectangle drawing functionality:
 * - Click to set first corner
 * - Move to preview rectangle
 * - Click to set opposite corner and complete rectangle
 * - Snap to grid support
 * - Visual preview
 */
class RectangleTool : public Tool
{
    Q_OBJECT

public:
    explicit RectangleTool(QObject* parent = nullptr);
    ~RectangleTool() override;

    // Tool identification
    QString name() const override;
    QString description() const override;

    // Tool lifecycle
    void activate() override;
    void reset() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Drawing overlay
    void drawOverlay(QPainter* painter) override;

private:
    QPointF m_startPoint;
    QPointF m_endPoint;
    bool m_firstPointSet;

    // Helper methods
    void startRectangle(const QPointF& point);
    void updatePreview(const QPointF& point);
    void finishRectangle(const QPointF& point);
    void createRectangle();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_RECTANGLETOOL_H
