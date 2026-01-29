/**
 * CircleTool.h
 *
 * Tool for drawing circles
 */

#ifndef PATTERNCAD_CIRCLETOOL_H
#define PATTERNCAD_CIRCLETOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {
namespace Tools {

/**
 * CircleTool provides circle drawing functionality:
 * - Click to set center point
 * - Move to preview circle radius
 * - Click to set radius and complete circle
 * - Snap to grid support
 * - Visual preview
 */
class CircleTool : public Tool
{
    Q_OBJECT

public:
    explicit CircleTool(QObject* parent = nullptr);
    ~CircleTool() override;

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
    QPointF m_centerPoint;
    QPointF m_radiusPoint;
    bool m_centerSet;

    // Helper methods
    void startCircle(const QPointF& point);
    void updatePreview(const QPointF& point);
    void finishCircle(const QPointF& point);
    void createCircle();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_CIRCLETOOL_H
