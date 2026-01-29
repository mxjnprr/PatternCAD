/**
 * MirrorTool.h
 *
 * Tool for creating mirrored copies of objects
 */

#ifndef PATTERNCAD_MIRRORTOOL_H
#define PATTERNCAD_MIRRORTOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {
namespace Tools {

/**
 * MirrorTool allows users to create mirrored copies of selected objects:
 * - Select mirror axis (horizontal, vertical, or custom)
 * - Visual preview of mirror axis
 * - Creates mirrored copies of selected objects
 */
class MirrorTool : public Tool
{
    Q_OBJECT

public:
    explicit MirrorTool(QObject* parent = nullptr);
    ~MirrorTool();

    QString name() const override { return "Mirror"; }
    QString description() const override { return "Create mirrored copies of objects"; }
    QCursor cursor() const override;

    void activate() override;
    void deactivate() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawOverlay(QPainter* painter) override;

private:
    enum class MirrorMode {
        Idle,
        SelectingAxis
    };

    enum class AxisType {
        Horizontal,
        Vertical,
        Custom
    };

    MirrorMode m_mode;
    AxisType m_axisType;

    QPointF m_axisPoint1;
    QPointF m_axisPoint2;
    QPointF m_currentPoint;

    // Helper methods
    void calculateDefaultAxis(AxisType type);
    void executeMirror();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_MIRRORTOOL_H
