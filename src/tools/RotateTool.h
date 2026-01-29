/**
 * RotateTool.h
 *
 * Tool for rotating selected objects
 */

#ifndef PATTERNCAD_ROTATETOOL_H
#define PATTERNCAD_ROTATETOOL_H

#include "Tool.h"
#include <QPointF>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
}

namespace Tools {

/**
 * RotateTool provides rotation functionality:
 * - Rotate selected objects interactively by dragging
 * - Rotation center defaults to selection center
 * - Shift+Drag snaps rotation to 15° increments
 * - Shows rotation angle overlay during drag
 */
class RotateTool : public Tool
{
    Q_OBJECT

public:
    explicit RotateTool(QObject* parent = nullptr);
    ~RotateTool();

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

    // Query if tool is active (rotating)
    bool hasVertexTargeted() const { return false; } // For consistency with SelectTool

public slots:
    // Handle numeric input from overlay
    void onNumericAngleEntered(double angle);

private:
    enum class RotateMode {
        Idle,
        SelectingCenter,  // Mouse down, selecting center position
        ReadyToRotate,    // Center set, waiting for rotation movement
        Rotating          // Actively rotating (preview mode)
    };

    // Helper methods
    QPointF calculateRotationCenter() const;
    double calculateAngle(const QPointF& from, const QPointF& to) const;
    double snapAngle(double angle) const;

    // State
    RotateMode m_mode;
    QPointF m_rotationCenter;
    QPointF m_startPoint;
    QPointF m_currentPoint;
    double m_currentAngle;
    QList<Geometry::GeometryObject*> m_objectsToRotate;
    bool m_snapEnabled;
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_ROTATETOOL_H
