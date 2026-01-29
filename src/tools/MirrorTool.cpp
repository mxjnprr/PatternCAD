/**
 * MirrorTool.cpp
 *
 * Implementation of MirrorTool
 */

#include "MirrorTool.h"
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

MirrorTool::MirrorTool(QObject* parent)
    : Tool(parent)
    , m_mode(MirrorMode::Idle)
    , m_axisType(AxisType::Horizontal)
    , m_axisPoint1(0, 0)
    , m_axisPoint2(0, 0)
    , m_currentPoint(0, 0)
{
}

MirrorTool::~MirrorTool()
{
}

QCursor MirrorTool::cursor() const
{
    return Qt::CrossCursor;
}

void MirrorTool::activate()
{
    Tool::activate();
    m_mode = MirrorMode::Idle;
    m_axisType = AxisType::Horizontal;

    // Check if there are selected objects
    if (m_document && !m_document->selectedObjects().isEmpty()) {
        // Calculate default horizontal axis through selection center
        calculateDefaultAxis(AxisType::Horizontal);
        m_mode = MirrorMode::SelectingAxis;

        if (m_canvas) {
            m_canvas->update();
        }
    }
}

void MirrorTool::deactivate()
{
    m_mode = MirrorMode::Idle;
    Tool::deactivate();
}

void MirrorTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_canvas || !m_document) {
        return;
    }

    m_currentPoint = m_canvas->mapToScene(event->pos());

    if (event->button() == Qt::LeftButton && m_axisType == AxisType::Custom) {
        if (m_mode == MirrorMode::Idle) {
            // Start defining custom axis
            m_axisPoint1 = m_currentPoint;
            m_axisPoint2 = m_currentPoint;
            m_mode = MirrorMode::SelectingAxis;
        }
    }
}

void MirrorTool::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_canvas) {
        return;
    }

    m_currentPoint = m_canvas->mapToScene(event->pos());

    if (m_mode == MirrorMode::SelectingAxis && m_axisType == AxisType::Custom) {
        // Update second point of custom axis
        m_axisPoint2 = m_currentPoint;
        m_canvas->update();
    }
}

void MirrorTool::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void MirrorTool::keyPressEvent(QKeyEvent* event)
{
    if (!m_document) {
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        // Cancel and return to idle
        m_mode = MirrorMode::Idle;
        if (m_canvas) {
            m_canvas->update();
        }
        return;
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Execute mirror operation
        executeMirror();
        return;
    }

    if (event->key() == Qt::Key_H) {
        // Switch to horizontal mirror
        m_axisType = AxisType::Horizontal;
        calculateDefaultAxis(AxisType::Horizontal);
        m_mode = MirrorMode::SelectingAxis;
        if (m_canvas) {
            m_canvas->update();
        }
        return;
    }

    if (event->key() == Qt::Key_V) {
        // Switch to vertical mirror
        m_axisType = AxisType::Vertical;
        calculateDefaultAxis(AxisType::Vertical);
        m_mode = MirrorMode::SelectingAxis;
        if (m_canvas) {
            m_canvas->update();
        }
        return;
    }

    if (event->key() == Qt::Key_C) {
        // Switch to custom mirror axis
        m_axisType = AxisType::Custom;
        m_mode = MirrorMode::Idle;
        if (m_canvas) {
            m_canvas->update();
        }
        return;
    }
}

void MirrorTool::drawOverlay(QPainter* painter)
{
    if (m_mode != MirrorMode::SelectingAxis) {
        return;
    }

    painter->save();

    // Draw mirror axis
    QPen axisPen(QColor(0, 128, 255), 2.0, Qt::DashLine);
    painter->setPen(axisPen);
    painter->drawLine(m_axisPoint1, m_axisPoint2);

    // Draw axis endpoints
    painter->setBrush(QColor(0, 128, 255));
    double pointSize = 5.0;
    painter->drawEllipse(m_axisPoint1, pointSize, pointSize);
    painter->drawEllipse(m_axisPoint2, pointSize, pointSize);

    // Draw axis type label
    QString axisLabel;
    switch (m_axisType) {
        case AxisType::Horizontal:
            axisLabel = "Horizontal Mirror (H)";
            break;
        case AxisType::Vertical:
            axisLabel = "Vertical Mirror (V)";
            break;
        case AxisType::Custom:
            axisLabel = "Custom Mirror (C)";
            break;
    }

    // Draw label with background
    QPointF midpoint = (m_axisPoint1 + m_axisPoint2) / 2.0;
    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    QFontMetrics metrics(font);
    QRectF textRect = metrics.boundingRect(axisLabel);
    textRect.moveCenter(midpoint + QPointF(0, -20));
    textRect.adjust(-5, -2, 5, 2);

    painter->setBrush(QColor(255, 255, 255, 200));
    painter->setPen(Qt::NoPen);
    painter->drawRect(textRect);

    painter->setPen(Qt::black);
    painter->drawText(textRect, Qt::AlignCenter, axisLabel);

    // Draw instructions
    QString instructions = "Enter: Mirror | H: Horizontal | V: Vertical | C: Custom | Esc: Cancel";
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

void MirrorTool::calculateDefaultAxis(AxisType type)
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

    QPointF center = boundingBox.center();

    // Define axis based on type
    if (type == AxisType::Horizontal) {
        // Horizontal mirror = vertical axis through center
        m_axisPoint1 = QPointF(center.x(), boundingBox.top() - 100);
        m_axisPoint2 = QPointF(center.x(), boundingBox.bottom() + 100);
    } else if (type == AxisType::Vertical) {
        // Vertical mirror = horizontal axis through center
        m_axisPoint1 = QPointF(boundingBox.left() - 100, center.y());
        m_axisPoint2 = QPointF(boundingBox.right() + 100, center.y());
    }
}

void MirrorTool::executeMirror()
{
    if (!m_document) {
        return;
    }

    QList<Geometry::GeometryObject*> selectedObjects = m_document->selectedObjects();
    if (selectedObjects.isEmpty()) {
        return;
    }

    // Create mirror command
    auto* command = new MirrorObjectsCommand(m_document, selectedObjects,
                                            m_axisPoint1, m_axisPoint2);

    if (m_document->undoStack()) {
        m_document->undoStack()->push(command);
    } else {
        command->redo();
        delete command;
    }

    // Clear selection and select the mirrored objects
    m_document->clearSelection();
    QList<Geometry::GeometryObject*> mirroredObjects = command->mirroredObjects();
    m_document->setSelectedObjects(mirroredObjects);

    // Return to idle mode (stay in Mirror tool for potential further mirroring)
    m_mode = MirrorMode::Idle;
    m_axisType = AxisType::Horizontal;
    calculateDefaultAxis(AxisType::Horizontal);
    m_mode = MirrorMode::SelectingAxis;

    if (m_canvas) {
        m_canvas->update();
    }
}

} // namespace Tools
} // namespace PatternCAD
