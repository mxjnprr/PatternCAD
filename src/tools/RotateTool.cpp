/**
 * RotateTool.cpp
 *
 * Implementation of RotateTool
 */

#include "RotateTool.h"
#include "core/Document.h"
#include "core/Commands.h"
#include "geometry/GeometryObject.h"
#include "ui/Canvas.h"
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include <QKeyEvent>

namespace PatternCAD {
namespace Tools {

RotateTool::RotateTool(QObject* parent)
    : Tool(parent)
    , m_mode(RotateMode::Idle)
    , m_rotationCenter(0, 0)
    , m_currentAngle(0.0)
    , m_snapEnabled(false)
{
}

RotateTool::~RotateTool()
{
}

QString RotateTool::name() const
{
    return "Rotate";
}

QString RotateTool::description() const
{
    return tr("Rotate selected objects (Ctrl+R)");
}

QCursor RotateTool::cursor() const
{
    return Qt::CrossCursor;
}

void RotateTool::activate()
{
    Tool::activate();
    reset();
}

void RotateTool::reset()
{
    m_mode = RotateMode::Idle;
    m_currentAngle = 0.0;
    m_snapEnabled = false;
    m_objectsToRotate.clear();
}

void RotateTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_canvas || !m_canvas->document()) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        Document* doc = m_canvas->document();
        QList<Geometry::GeometryObject*> selectedObjects = doc->selectedObjects();

        if (selectedObjects.isEmpty()) {
            // Nothing selected, do nothing
            return;
        }

        // Store objects to rotate
        m_objectsToRotate = selectedObjects;

        // Calculate rotation center (center of selection bounding box)
        m_rotationCenter = calculateRotationCenter();

        // Start rotation
        m_mode = RotateMode::Rotating;
        m_startPoint = m_canvas->mapToScene(event->pos());
        m_currentPoint = m_startPoint;
        m_currentAngle = 0.0;
        m_snapEnabled = (event->modifiers() & Qt::ShiftModifier);
    }
}

void RotateTool::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mode == RotateMode::Rotating) {
        m_currentPoint = m_canvas->mapToScene(event->pos());
        m_snapEnabled = (event->modifiers() & Qt::ShiftModifier);

        // Calculate angle between start and current point relative to center
        double startAngle = calculateAngle(m_rotationCenter, m_startPoint);
        double currentAngleRaw = calculateAngle(m_rotationCenter, m_currentPoint);
        m_currentAngle = currentAngleRaw - startAngle;

        // Normalize to -180 to 180
        while (m_currentAngle > 180.0) m_currentAngle -= 360.0;
        while (m_currentAngle < -180.0) m_currentAngle += 360.0;

        // Apply snap if enabled
        if (m_snapEnabled) {
            m_currentAngle = snapAngle(m_currentAngle);
        }

        // Request canvas redraw to show overlay
        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void RotateTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_mode == RotateMode::Rotating) {
        // Apply rotation if angle is non-zero
        if (qAbs(m_currentAngle) > 0.01) {
            Document* doc = m_canvas->document();
            if (doc && !m_objectsToRotate.isEmpty()) {
                // Create rotation command
                auto* cmd = new RotateObjectsCommand(m_objectsToRotate,
                                                     m_currentAngle,
                                                     m_rotationCenter);
                doc->undoStack()->push(cmd);
            }
        }

        // Reset state
        reset();

        // Redraw canvas
        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void RotateTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        // Cancel rotation
        reset();
        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void RotateTool::drawOverlay(QPainter* painter)
{
    if (m_mode == RotateMode::Rotating) {
        painter->save();

        // Draw rotation center indicator
        QPen centerPen(QColor(255, 0, 0), 2);
        painter->setPen(centerPen);
        painter->setBrush(Qt::NoBrush);

        double centerRadius = 8.0 / m_canvas->zoomLevel();
        painter->drawEllipse(m_rotationCenter, centerRadius, centerRadius);
        painter->drawLine(QPointF(m_rotationCenter.x() - centerRadius, m_rotationCenter.y()),
                         QPointF(m_rotationCenter.x() + centerRadius, m_rotationCenter.y()));
        painter->drawLine(QPointF(m_rotationCenter.x(), m_rotationCenter.y() - centerRadius),
                         QPointF(m_rotationCenter.x(), m_rotationCenter.y() + centerRadius));

        // Draw rotation arc
        QPen arcPen(QColor(0, 120, 215), 2);
        arcPen.setStyle(Qt::DashLine);
        painter->setPen(arcPen);

        double arcRadius = QLineF(m_rotationCenter, m_startPoint).length();
        QRectF arcRect(m_rotationCenter.x() - arcRadius,
                      m_rotationCenter.y() - arcRadius,
                      arcRadius * 2,
                      arcRadius * 2);

        double startAngle = calculateAngle(m_rotationCenter, m_startPoint);
        painter->drawArc(arcRect,
                        static_cast<int>(-startAngle * 16),
                        static_cast<int>(-m_currentAngle * 16));

        // Draw angle text
        QString angleText = QString("%1°").arg(m_currentAngle, 0, 'f', 1);
        QPointF textPos = m_rotationCenter + QPointF(20 / m_canvas->zoomLevel(),
                                                      -20 / m_canvas->zoomLevel());

        // Draw text background
        QFont font = painter->font();
        font.setPointSize(12);
        painter->setFont(font);
        QFontMetrics fm(font);
        QRect textRect = fm.boundingRect(angleText);
        textRect.moveTopLeft(textPos.toPoint());
        textRect.adjust(-4, -2, 4, 2);

        painter->setBrush(QColor(255, 255, 255, 200));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);

        // Draw text
        painter->setPen(Qt::black);
        painter->drawText(textPos, angleText);

        painter->restore();
    }
}

QPointF RotateTool::calculateRotationCenter() const
{
    if (m_objectsToRotate.isEmpty()) {
        return QPointF(0, 0);
    }

    // Calculate bounding box of all selected objects
    QRectF bounds = m_objectsToRotate.first()->boundingRect();
    for (int i = 1; i < m_objectsToRotate.size(); ++i) {
        bounds = bounds.united(m_objectsToRotate[i]->boundingRect());
    }

    return bounds.center();
}

double RotateTool::calculateAngle(const QPointF& from, const QPointF& to) const
{
    // Calculate angle in degrees from 'from' to 'to'
    QPointF delta = to - from;
    double angleRadians = qAtan2(delta.y(), delta.x());
    return qRadiansToDegrees(angleRadians);
}

double RotateTool::snapAngle(double angle) const
{
    // Snap to 15° increments
    const double snapIncrement = 15.0;
    return qRound(angle / snapIncrement) * snapIncrement;
}

} // namespace Tools
} // namespace PatternCAD
