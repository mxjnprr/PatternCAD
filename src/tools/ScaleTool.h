/**
 * ScaleTool.h
 *
 * Tool for scaling objects uniformly and non-uniformly
 */

#ifndef PATTERNCAD_SCALETOOL_H
#define PATTERNCAD_SCALETOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {
namespace Tools {

/**
 * ScaleTool allows users to scale selected objects:
 * - Interactive scaling by dragging
 * - Uniform and non-uniform scaling modes
 * - Visual preview of scale factors
 * - Scale origin at selection center
 */
class ScaleTool : public Tool
{
    Q_OBJECT

public:
    explicit ScaleTool(QObject* parent = nullptr);
    ~ScaleTool();

    QString name() const override { return "Scale"; }
    QString description() const override { return "Scale objects uniformly or non-uniformly"; }
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
    enum class ScaleMode {
        Idle,
        Scaling
    };

    ScaleMode m_mode;
    bool m_uniformScale;

    QPointF m_scaleOrigin;
    QPointF m_startPoint;
    QPointF m_currentPoint;

    double m_scaleX;
    double m_scaleY;
    double m_initialDistance;

    // Helper methods
    void calculateScaleOrigin();
    void calculateScaleFactors();
    void executeScale();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_SCALETOOL_H
