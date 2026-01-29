/**
 * ScaleTool.cpp
 *
 * Implementation of ScaleTool
 */

#include "ScaleTool.h"
#include "../core/Document.h"
#include "../core/Commands.h"
#include "../geometry/GeometryObject.h"
#include "../ui/Canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtMath>

namespace PatternCAD {
namespace Tools {

ScaleTool::ScaleTool(QObject* parent)
    : Tool(parent)
    , m_mode(ScaleMode::Idle)
    , m_uniformScale(true)
    , m_scaleOrigin(0, 0)
    , m_startPoint(0, 0)
    , m_currentPoint(0, 0)
    , m_scaleX(1.0)
    , m_scaleY(1.0)
    , m_initialDistance(1.0)
{
}

ScaleTool::~ScaleTool()
{
}

QCursor ScaleTool::cursor() const
{
    return Qt::SizeAllCursor;
}

void ScaleTool::activate()
{
    Tool::activate();
    m_mode = ScaleMode::Idle;
    m_uniformScale = true;
    m_scaleX = 1.0;

    // Show usage instructions
    if (m_document && !m_document->selectedObjects().isEmpty()) {
        emit statusMessage(tr("Scale Tool: Click to set origin | Drag to scale | U=Toggle uniform/non-uniform | Enter=Apply | Esc=Cancel"));
    } else {
        emit statusMessage(tr("Scale Tool: Select objects first"));
    }
    m_scaleY = 1.0;

    // Check if there are selected objects
    if (m_document && !m_document->selectedObjects().isEmpty()) {
        calculateScaleOrigin();
    }

    if (m_canvas) {
        m_canvas->update();
    }
}

void ScaleTool::deactivate()
{
    m_mode = ScaleMode::Idle;
    Tool::deactivate();
}

void ScaleTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_canvas || !m_document) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if (m_document->selectedObjects().isEmpty()) {
            return;
        }

        m_startPoint = m_canvas->mapToScene(event->pos());
        m_currentPoint = m_startPoint;
        m_mode = ScaleMode::Scaling;

        // Calculate initial distance from origin
        QPointF delta = m_startPoint - m_scaleOrigin;
        m_initialDistance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
        if (m_initialDistance < 1.0) {
            m_initialDistance = 1.0;
        }

        m_scaleX = 1.0;
        m_scaleY = 1.0;

        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void ScaleTool::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_canvas) {
        return;
    }

    m_currentPoint = m_canvas->mapToScene(event->pos());

    if (m_mode == ScaleMode::Scaling) {
        // Update uniform scale flag based on Shift modifier
        m_uniformScale = !(event->modifiers() & Qt::ShiftModifier);

        calculateScaleFactors();

        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void ScaleTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_canvas || !m_document) {
        return;
    }

    if (event->button() == Qt::LeftButton && m_mode == ScaleMode::Scaling) {
        // Execute scale operation
        executeScale();

        // Reset to idle
        m_mode = ScaleMode::Idle;
        m_scaleX = 1.0;
        m_scaleY = 1.0;

        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void ScaleTool::keyPressEvent(QKeyEvent* event)
{
    if (!m_document) {
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        // Cancel scaling
        m_mode = ScaleMode::Idle;
        m_scaleX = 1.0;
        m_scaleY = 1.0;
        if (m_canvas) {
            m_canvas->update();
        }
        return;
    }

    if (event->key() == Qt::Key_Shift) {
        // Toggle to non-uniform scale
        m_uniformScale = false;
        if (m_mode == ScaleMode::Scaling) {
            calculateScaleFactors();
            if (m_canvas) {
                m_canvas->update();
            }
        }
    }
}

void ScaleTool::drawOverlay(QPainter* painter)
{
    if (m_mode != ScaleMode::Scaling) {
        return;
    }

    painter->save();

    // Draw scale origin
    QPen originPen(QColor(255, 128, 0), 2.0);
    painter->setPen(originPen);
    painter->setBrush(QColor(255, 128, 0));
    double pointSize = 6.0;
    painter->drawEllipse(m_scaleOrigin, pointSize, pointSize);

    // Draw line from origin to current point
    QPen linePen(QColor(255, 128, 0), 1.0, Qt::DashLine);
    painter->setPen(linePen);
    painter->drawLine(m_scaleOrigin, m_currentPoint);

    // Draw scale factors text
    QString scaleText;
    if (m_uniformScale) {
        scaleText = QString("Scale: %1%").arg(m_scaleX * 100.0, 0, 'f', 1);
    } else {
        scaleText = QString("Scale X: %1%  Y: %2%")
            .arg(m_scaleX * 100.0, 0, 'f', 1)
            .arg(m_scaleY * 100.0, 0, 'f', 1);
    }

    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    QFontMetrics metrics(font);
    QRectF textRect = metrics.boundingRect(scaleText);

    // Position text near origin
    QPointF textPos = m_scaleOrigin + QPointF(15, -15);
    textRect.moveCenter(textPos);
    textRect.adjust(-5, -2, 5, 2);

    // Draw background
    painter->setBrush(QColor(255, 255, 255, 220));
    painter->setPen(Qt::NoPen);
    painter->drawRect(textRect);

    // Draw text
    painter->setPen(Qt::black);
    painter->drawText(textRect, Qt::AlignCenter, scaleText);

    // Draw instructions
    QString instructions = m_uniformScale
        ? "Shift: Non-uniform | Esc: Cancel"
        : "Release Shift: Uniform | Esc: Cancel";

    textRect = metrics.boundingRect(instructions);
    textRect.moveTopLeft(QPointF(10, 10));
    textRect.adjust(-5, -2, 5, 2);

    painter->setBrush(QColor(255, 255, 255, 220));
    painter->setPen(Qt::NoPen);
    painter->drawRect(textRect);

    painter->setPen(Qt::black);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, instructions);

    painter->restore();
}

void ScaleTool::calculateScaleOrigin()
{
    if (!m_document) {
        return;
    }

    // Get selection bounding box
    QList<Geometry::GeometryObject*> selectedObjects = m_document->selectedObjects();
    if (selectedObjects.isEmpty()) {
        return;
    }

    // Calculate bounding box of all selected objects
    QRectF boundingBox = selectedObjects.first()->boundingRect();
    for (int i = 1; i < selectedObjects.size(); ++i) {
        boundingBox = boundingBox.united(selectedObjects[i]->boundingRect());
    }

    m_scaleOrigin = boundingBox.center();
}

void ScaleTool::calculateScaleFactors()
{
    // Calculate current distance from origin
    QPointF delta = m_currentPoint - m_scaleOrigin;
    double currentDistance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());

    if (m_uniformScale) {
        // Uniform scaling based on distance from origin
        m_scaleX = m_scaleY = currentDistance / m_initialDistance;

        // Clamp to reasonable values
        if (m_scaleX < 0.01) m_scaleX = 0.01;
        if (m_scaleX > 100.0) m_scaleX = 100.0;
        m_scaleY = m_scaleX;
    } else {
        // Non-uniform scaling based on X and Y distances
        QPointF initialDelta = m_startPoint - m_scaleOrigin;

        if (qAbs(initialDelta.x()) > 0.1) {
            m_scaleX = delta.x() / initialDelta.x();
        } else {
            m_scaleX = 1.0;
        }

        if (qAbs(initialDelta.y()) > 0.1) {
            m_scaleY = delta.y() / initialDelta.y();
        } else {
            m_scaleY = 1.0;
        }

        // Clamp to reasonable values
        if (m_scaleX < 0.01) m_scaleX = 0.01;
        if (m_scaleX > 100.0) m_scaleX = 100.0;
        if (m_scaleY < 0.01) m_scaleY = 0.01;
        if (m_scaleY > 100.0) m_scaleY = 100.0;
    }
}

void ScaleTool::executeScale()
{
    if (!m_document) {
        return;
    }

    QList<Geometry::GeometryObject*> selectedObjects = m_document->selectedObjects();
    if (selectedObjects.isEmpty()) {
        return;
    }

    // Don't scale if factors are essentially 1.0 (no change)
    if (qAbs(m_scaleX - 1.0) < 0.001 && qAbs(m_scaleY - 1.0) < 0.001) {
        return;
    }

    // Create scale command
    auto* command = new ScaleObjectsCommand(selectedObjects, m_scaleX, m_scaleY, m_scaleOrigin);

    if (m_document->undoStack()) {
        m_document->undoStack()->push(command);
    } else {
        command->redo();
        delete command;
    }

    emit statusMessage(tr("Scale applied"));
    emit objectCreated();  // Signal to return to Select tool
}

} // namespace Tools
} // namespace PatternCAD
