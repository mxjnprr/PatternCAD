/**
 * LineTool.h
 *
 * Tool for drawing line segments
 */

#ifndef PATTERNCAD_LINETOOL_H
#define PATTERNCAD_LINETOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {

namespace Geometry {
    class Line;
}

namespace Tools {

/**
 * LineTool provides line drawing functionality:
 * - Click to set start point
 * - Move to preview line
 * - Click to set end point and complete line
 * - Snap to grid support
 * - Visual preview
 */
class LineTool : public Tool
{
    Q_OBJECT

public:
    explicit LineTool(QObject* parent = nullptr);
    ~LineTool();

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
    QPointF m_currentPoint;
    bool m_hasStartPoint;

    // Helper methods
    void startLine(const QPointF& point);
    void updatePreview(const QPointF& point);
    void finishLine(const QPointF& point);
    void createLine();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_LINETOOL_H
