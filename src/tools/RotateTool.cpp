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

        if (m_mode == RotateMode::Idle) {
            QList<Geometry::GeometryObject*> selectedObjects = doc->selectedObjects();

            if (selectedObjects.isEmpty()) {
                // Nothing selected, do nothing
                return;
            }

            // Store objects to rotate
            m_objectsToRotate = selectedObjects;

            // First click down: start selecting rotation center
            m_mode = RotateMode::SelectingCenter;
            m_rotationCenter = m_canvas->mapToScene(event->pos());
            m_canvas->update();
        }
        else if (m_mode == RotateMode::Rotating) {
            // Click while rotating: validate rotation
            if (qAbs(m_currentAngle) > 0.01) {
                if (doc && !m_objectsToRotate.isEmpty()) {
                    auto* cmd = new RotateObjectsCommand(m_objectsToRotate,
                                                         m_currentAngle,
                                                         m_rotationCenter);
                    doc->undoStack()->push(cmd);
                }
            }
            reset();
            m_canvas->update();
        }
    }
}

void RotateTool::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mode == RotateMode::SelectingCenter) {
        // Update rotation center position to follow mouse while button down
        m_rotationCenter = m_canvas->mapToScene(event->pos());
        if (m_canvas) {
            m_canvas->update();
        }
    }
    else if (m_mode == RotateMode::ReadyToRotate || m_mode == RotateMode::Rotating) {
        // Mouse movement (no button down) shows rotation preview
        m_mode = RotateMode::Rotating;
        m_currentPoint = m_canvas->mapToScene(event->pos());
        m_snapEnabled = (event->modifiers() & Qt::ShiftModifier);

        // Calculate angle from center to current mouse position
        m_currentAngle = calculateAngle(m_rotationCenter, m_currentPoint);

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
    if (event->button() == Qt::LeftButton && m_mode == RotateMode::SelectingCenter) {
        // First click released: center is set, ready to rotate
        m_mode = RotateMode::ReadyToRotate;
        m_currentAngle = 0.0;
        m_canvas->update();
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
        event->accept();
    }
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Enter key: validate rotation
        if (m_mode == RotateMode::Rotating) {
            if (qAbs(m_currentAngle) > 0.01) {
                Document* doc = m_canvas->document();
                if (doc && !m_objectsToRotate.isEmpty()) {
                    auto* cmd = new RotateObjectsCommand(m_objectsToRotate,
                                                         m_currentAngle,
                                                         m_rotationCenter);
                    doc->undoStack()->push(cmd);
                }
            }
            reset();
            m_canvas->update();
            event->accept();
        }
    }
    else if (event->key() == Qt::Key_Tab) {
        // Tab key: open numeric input for angle
        if (m_mode == RotateMode::ReadyToRotate || m_mode == RotateMode::Rotating) {
            // Use existing dimensionInputRequested signal with mode "angle"
            // First parameter is the main input field value (angle in this case)
            emit dimensionInputRequested("angle", m_currentAngle, 0.0);
            event->accept();
        }
    }
}

void RotateTool::drawOverlay(QPainter* painter)
{
    if (m_mode == RotateMode::SelectingCenter) {
        painter->save();

        // Draw rotation center indicator (crosshair)
        QPen centerPen(QColor(255, 0, 0), 2);
        painter->setPen(centerPen);
        painter->setBrush(Qt::NoBrush);

        double centerRadius = 10.0 / m_canvas->zoomLevel();
        painter->drawEllipse(m_rotationCenter, centerRadius, centerRadius);
        painter->drawLine(QPointF(m_rotationCenter.x() - centerRadius * 2, m_rotationCenter.y()),
                         QPointF(m_rotationCenter.x() + centerRadius * 2, m_rotationCenter.y()));
        painter->drawLine(QPointF(m_rotationCenter.x(), m_rotationCenter.y() - centerRadius * 2),
                         QPointF(m_rotationCenter.x(), m_rotationCenter.y() + centerRadius * 2));

        // Draw hint text
        QFont font = painter->font();
        font.setPointSize(11);
        painter->setFont(font);
        QString hintText = tr("Release to set rotation center");
        QFontMetrics fm(font);
        QRect textRect = fm.boundingRect(hintText);
        QPointF textPos = m_rotationCenter + QPointF(20 / m_canvas->zoomLevel(), -20 / m_canvas->zoomLevel());
        textRect.moveTopLeft(textPos.toPoint());
        textRect.adjust(-4, -2, 4, 2);

        painter->setBrush(QColor(255, 255, 255, 220));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        painter->setPen(Qt::black);
        painter->drawText(textRect, Qt::AlignCenter, hintText);

        painter->restore();
    }
    else if (m_mode == RotateMode::ReadyToRotate) {
        painter->save();

        // Draw rotation center indicator (fixed)
        QPen centerPen(QColor(255, 0, 0), 2);
        painter->setPen(centerPen);
        painter->setBrush(Qt::red);

        double centerRadius = 8.0 / m_canvas->zoomLevel();
        painter->drawEllipse(m_rotationCenter, centerRadius, centerRadius);

        // Draw hint text
        QFont font = painter->font();
        font.setPointSize(11);
        painter->setFont(font);
        QString hintText = tr("Move mouse to rotate (Tab for value, Enter to confirm)");
        QFontMetrics fm(font);
        QRect textRect = fm.boundingRect(hintText);
        QPointF textPos = m_rotationCenter + QPointF(20 / m_canvas->zoomLevel(), -20 / m_canvas->zoomLevel());
        textRect.moveTopLeft(textPos.toPoint());
        textRect.adjust(-4, -2, 4, 2);

        painter->setBrush(QColor(255, 255, 255, 220));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        painter->setPen(Qt::black);
        painter->drawText(textRect, Qt::AlignCenter, hintText);

        painter->restore();
    }
    else if (m_mode == RotateMode::Rotating) {
        painter->save();

        // Draw preview of rotated objects (semi-transparent)
        painter->setOpacity(0.5);
        painter->save();
        painter->translate(m_rotationCenter);
        painter->rotate(m_currentAngle);
        painter->translate(-m_rotationCenter);

        for (auto* obj : m_objectsToRotate) {
            if (obj) {
                obj->draw(painter, QColor(0, 150, 255));
            }
        }
        painter->restore();
        painter->setOpacity(1.0);

        // Draw rotation center indicator
        QPen centerPen(QColor(255, 0, 0), 3);
        painter->setPen(centerPen);
        painter->setBrush(Qt::red);

        double centerRadius = 6.0 / m_canvas->zoomLevel();
        painter->drawEllipse(m_rotationCenter, centerRadius, centerRadius);

        // Draw line from center to current mouse position
        QPen linePen(QColor(0, 120, 215), 2);
        linePen.setStyle(Qt::DashLine);
        painter->setPen(linePen);
        painter->drawLine(m_rotationCenter, m_currentPoint);

        // Draw angle text
        QString angleText = QString("%1°").arg(m_currentAngle, 0, 'f', 1);
        if (m_snapEnabled) {
            angleText = "🔒 " + angleText;
        }
        QPointF textPos = m_rotationCenter + QPointF(20 / m_canvas->zoomLevel(),
                                                      -20 / m_canvas->zoomLevel());

        // Draw text background
        QFont font = painter->font();
        font.setPointSize(12);
        font.setBold(true);
        painter->setFont(font);
        QFontMetrics fm(font);
        QRect textRect = fm.boundingRect(angleText);
        textRect.moveTopLeft(textPos.toPoint());
        textRect.adjust(-4, -2, 4, 2);

        painter->setBrush(QColor(255, 255, 255, 220));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);

        // Draw text
        painter->setPen(Qt::black);
        painter->drawText(textRect, Qt::AlignCenter, angleText);

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

void RotateTool::onNumericAngleEntered(double angle)
{
    if (m_mode == RotateMode::ReadyToRotate || m_mode == RotateMode::Rotating) {
        // Set the angle and apply rotation
        m_currentAngle = angle;

        // Apply rotation
        if (!m_objectsToRotate.isEmpty() && m_canvas && m_canvas->document()) {
            Document* doc = m_canvas->document();
            auto* cmd = new RotateObjectsCommand(m_objectsToRotate,
                                                 m_currentAngle,
                                                 m_rotationCenter);
            doc->undoStack()->push(cmd);
        }

        // Reset state
        reset();
        if (m_canvas) {
            m_canvas->update();
        }
    }
}

} // namespace Tools
} // namespace PatternCAD
