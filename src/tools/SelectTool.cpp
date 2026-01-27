/**
 * SelectTool.cpp
 *
 * Implementation of SelectTool
 */

#include "SelectTool.h"
#include "core/Document.h"
#include "geometry/GeometryObject.h"
#include <QPainter>

namespace PatternCAD {
namespace Tools {

SelectTool::SelectTool(QObject* parent)
    : Tool(parent)
    , m_mode(SelectMode::None)
    , m_multiSelect(false)
{
}

SelectTool::~SelectTool()
{
}

QString SelectTool::name() const
{
    return "Select";
}

QString SelectTool::description() const
{
    return "Select and move objects";
}

QCursor SelectTool::cursor() const
{
    if (m_mode == SelectMode::Moving) {
        return QCursor(Qt::ClosedHandCursor);
    }
    return QCursor(Qt::ArrowCursor);
}

void SelectTool::activate()
{
    Tool::activate();
    m_mode = SelectMode::None;
    showStatusMessage("Click to select objects, drag to move");
}

void SelectTool::reset()
{
    Tool::reset();
    m_mode = SelectMode::None;
    m_multiSelect = false;
}

void SelectTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) {
        return;
    }

    m_startPoint = event->position().toPoint();
    m_currentPoint = m_startPoint;
    m_lastPoint = m_startPoint;
    m_multiSelect = (event->modifiers() & Qt::ControlModifier);

    // Check if clicking on selected object
    Geometry::GeometryObject* obj = findObjectAt(m_startPoint);

    if (obj && obj->isSelected()) {
        // Start moving selected objects
        m_mode = SelectMode::Moving;
        startMoving();
    } else {
        // Start new selection
        m_mode = SelectMode::Selecting;
        selectObjectAt(m_startPoint, m_multiSelect);
    }

    event->accept();
}

void SelectTool::mouseMoveEvent(QMouseEvent* event)
{
    m_currentPoint = event->position().toPoint();

    if (m_mode == SelectMode::Moving) {
        // Move selected objects
        QPointF delta = m_currentPoint - m_lastPoint;
        updateMove(delta);
        m_lastPoint = m_currentPoint;
    } else if (m_mode == SelectMode::Selecting) {
        // Update selection rectangle
        m_selectionRect = QRectF(m_startPoint, m_currentPoint).normalized();
    }

    event->accept();
}

void SelectTool::mouseReleaseEvent(QMouseEvent* event)
{
    m_currentPoint = event->position().toPoint();

    if (m_mode == SelectMode::Moving) {
        finishMove();
    } else if (m_mode == SelectMode::Selecting) {
        // Complete rectangular selection if we dragged
        QPointF delta = m_currentPoint - m_startPoint;
        double distance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (distance > 5.0) {
            selectObjectsInRect(m_selectionRect, m_multiSelect);
        }
    }

    m_mode = SelectMode::None;
    m_selectionRect = QRectF();
    event->accept();
}

void SelectTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        // Delete selected objects
        if (m_document) {
            auto selected = m_document->selectedObjects();
            for (auto* obj : selected) {
                m_document->removeObject(obj);
            }
        }
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void SelectTool::drawOverlay(QPainter* painter)
{
    if (m_mode == SelectMode::Selecting && !m_selectionRect.isEmpty()) {
        // Draw selection rectangle
        painter->save();
        QPen pen(Qt::blue, 1, Qt::DashLine);
        painter->setPen(pen);
        painter->setBrush(QColor(100, 149, 237, 50)); // Semi-transparent blue
        painter->drawRect(m_selectionRect);
        painter->restore();
    }
}

void SelectTool::selectObjectAt(const QPointF& point, bool addToSelection)
{
    if (!m_document) {
        return;
    }

    Geometry::GeometryObject* obj = findObjectAt(point);

    if (!addToSelection) {
        m_document->clearSelection();
    }

    if (obj) {
        QList<Geometry::GeometryObject*> selected = m_document->selectedObjects();
        if (selected.contains(obj) && addToSelection) {
            // Deselect if already selected
            selected.removeAll(obj);
        } else {
            // Add to selection
            selected.append(obj);
        }
        m_document->setSelectedObjects(selected);
    }
}

void SelectTool::selectObjectsInRect(const QRectF& rect, bool addToSelection)
{
    if (!m_document) {
        return;
    }

    if (!addToSelection) {
        m_document->clearSelection();
    }

    QList<Geometry::GeometryObject*> objects = findObjectsInRect(rect);
    if (!objects.isEmpty()) {
        QList<Geometry::GeometryObject*> selected = m_document->selectedObjects();
        for (auto* obj : objects) {
            if (!selected.contains(obj)) {
                selected.append(obj);
            }
        }
        m_document->setSelectedObjects(selected);
    }
}

void SelectTool::startMoving()
{
    showStatusMessage("Moving selected objects");
}

void SelectTool::updateMove(const QPointF& delta)
{
    if (!m_document) {
        return;
    }

    // TODO: Apply delta to selected objects
    // for (auto* obj : m_document->selectedObjects()) {
    //     obj->translate(delta);
    // }
}

void SelectTool::finishMove()
{
    showStatusMessage("Move complete");
}

Geometry::GeometryObject* SelectTool::findObjectAt(const QPointF& point) const
{
    if (!m_document) {
        return nullptr;
    }

    // Search in reverse order (top to bottom)
    auto objects = m_document->objects();
    for (int i = objects.size() - 1; i >= 0; --i) {
        if (objects[i]->contains(point)) {
            return objects[i];
        }
    }

    return nullptr;
}

QList<Geometry::GeometryObject*> SelectTool::findObjectsInRect(const QRectF& rect) const
{
    QList<Geometry::GeometryObject*> result;

    if (!m_document) {
        return result;
    }

    for (auto* obj : m_document->objects()) {
        if (rect.intersects(obj->boundingRect())) {
            result.append(obj);
        }
    }

    return result;
}

} // namespace Tools
} // namespace PatternCAD
