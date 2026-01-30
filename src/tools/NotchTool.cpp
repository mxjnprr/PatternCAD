/**
 * NotchTool.cpp
 *
 * Implementation of NotchTool for placing and managing notches
 */

#include "NotchTool.h"
#include "geometry/GeometryObject.h"
#include "geometry/Polyline.h"
#include "geometry/Notch.h"
#include "core/Document.h"
#include "core/Commands.h"
#include "ui/Canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <cmath>

namespace PatternCAD {
namespace Tools {

NotchTool::NotchTool(QObject* parent)
    : Tool(parent)
    , m_mode(Mode::Idle)
    , m_notchStyle(NotchStyle::VNotch)
    , m_notchDepth(5.0)  // Default 5mm
    , m_hoveredPolyline(nullptr)
    , m_hoveredSegment(-1)
    , m_hoveredPosition(0.0)
    , m_selectedNotch(nullptr)
    , m_selectedPolyline(nullptr)
    , m_dragNotch(nullptr)
    , m_dragPolyline(nullptr)
    , m_contextMenu(nullptr)
{
    // Create context menu
    m_contextMenu = new QMenu();
    
    QAction* vNotchAction = m_contextMenu->addAction("V-Notch");
    QAction* slitAction = m_contextMenu->addAction("Slit");
    QAction* dotAction = m_contextMenu->addAction("Dot");
    m_contextMenu->addSeparator();
    QAction* deleteAction = m_contextMenu->addAction("Delete");
    
    connect(vNotchAction, &QAction::triggered, this, &NotchTool::onStyleVNotch);
    connect(slitAction, &QAction::triggered, this, &NotchTool::onStyleSlit);
    connect(dotAction, &QAction::triggered, this, &NotchTool::onStyleDot);
    connect(deleteAction, &QAction::triggered, this, &NotchTool::onDeleteNotch);
}

NotchTool::~NotchTool()
{
    delete m_contextMenu;
}

QString NotchTool::name() const
{
    return "Notch";
}

QString NotchTool::description() const
{
    return "Place notches on pattern edges (N)";
}

QCursor NotchTool::cursor() const
{
    return Qt::CrossCursor;
}

void NotchTool::activate()
{
    Tool::activate();
    reset();
    updateStatusMessage();
}

void NotchTool::deactivate()
{
    reset();
    Tool::deactivate();
}

void NotchTool::reset()
{
    m_mode = Mode::Idle;
    m_hoveredPolyline = nullptr;
    m_hoveredSegment = -1;
    m_hoveredPosition = 0.0;
    m_selectedNotch = nullptr;
    m_selectedPolyline = nullptr;
    m_dragNotch = nullptr;
    m_dragPolyline = nullptr;
    updateStatusMessage();
}

void NotchTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        // Check if clicking on an existing notch
        Geometry::Polyline* notchPolyline = nullptr;
        Notch* clickedNotch = findNotchAt(scenePos, &notchPolyline);
        
        if (clickedNotch) {
            // Select the notch for dragging or editing
            selectNotch(clickedNotch, notchPolyline);
            m_mode = Mode::Dragging;
            m_dragNotch = clickedNotch;
            m_dragPolyline = notchPolyline;
        } else if (m_hoveredPolyline && m_hoveredSegment >= 0) {
            // Place a new notch
            placeNotch();
        }
    } else if (event->button() == Qt::RightButton) {
        // Right-click context menu
        Geometry::Polyline* notchPolyline = nullptr;
        Notch* clickedNotch = findNotchAt(scenePos, &notchPolyline);
        
        if (clickedNotch) {
            selectNotch(clickedNotch, notchPolyline);
            showContextMenu(event->globalPosition().toPoint());
        } else {
            deselectNotch();
        }
    }

    if (m_canvas) m_canvas->update();
}

void NotchTool::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());

    if (m_mode == Mode::Dragging && m_dragNotch && m_dragPolyline) {
        // Update notch position while dragging
        int segmentIndex = -1;
        double position = 0.0;
        Geometry::Polyline* poly = findPolylineAt(scenePos, &segmentIndex, &position);
        
        if (poly == m_dragPolyline && segmentIndex >= 0) {
            // Update notch position on same polyline
            m_dragNotch->setSegmentIndex(segmentIndex);
            m_dragNotch->setPosition(position);
            m_document->setModified(true);
        }
    } else {
        // Update hover state
        int segmentIndex = -1;
        double position = 0.0;
        m_hoveredPolyline = findPolylineAt(scenePos, &segmentIndex, &position);
        m_hoveredSegment = segmentIndex;
        m_hoveredPosition = position;
        
        if (m_hoveredPolyline && m_hoveredSegment >= 0) {
            // Calculate hover point on edge
            auto vertices = m_hoveredPolyline->vertices();
            int nextIdx = (m_hoveredSegment + 1) % vertices.size();
            QPointF p1 = vertices[m_hoveredSegment].position;
            QPointF p2 = vertices[nextIdx].position;
            m_hoveredPoint = p1 + (p2 - p1) * m_hoveredPosition;
            m_mode = Mode::PreviewPlace;
        } else {
            m_mode = Mode::Idle;
        }
    }

    updateStatusMessage();
    if (m_canvas) m_canvas->update();
}

void NotchTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_mode == Mode::Dragging) {
        // Finish dragging
        m_mode = Mode::Idle;
        m_dragNotch = nullptr;
        m_dragPolyline = nullptr;
        updateStatusMessage();
        if (m_canvas) m_canvas->update();
    }
}

void NotchTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        deselectNotch();
        reset();
        if (m_canvas) m_canvas->update();
    } else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        deleteSelectedNotch();
    } else if (event->key() == Qt::Key_1) {
        setNotchStyle(NotchStyle::VNotch);
    } else if (event->key() == Qt::Key_2) {
        setNotchStyle(NotchStyle::Slit);
    } else if (event->key() == Qt::Key_3) {
        setNotchStyle(NotchStyle::Dot);
    }
}

void NotchTool::drawOverlay(QPainter* painter)
{
    if (!painter || !m_document) return;

    painter->save();

    // Draw preview notch at hover position
    if (m_mode == Mode::PreviewPlace && m_hoveredPolyline && m_hoveredSegment >= 0) {
        drawPreviewNotch(painter);
    }

    // Highlight selected notch
    if (m_selectedNotch && m_selectedPolyline) {
        drawNotchHighlight(painter, m_selectedNotch, m_selectedPolyline);
    }

    painter->restore();
}

void NotchTool::setNotchStyle(NotchStyle style)
{
    if (m_notchStyle != style) {
        m_notchStyle = style;
        emit notchStyleChanged(style);
        
        // Apply to selected notch
        if (m_selectedNotch) {
            m_selectedNotch->setStyle(style);
            m_document->setModified(true);
            if (m_canvas) m_canvas->update();
        }
        
        updateStatusMessage();
    }
}

void NotchTool::setNotchDepth(double depth)
{
    if (m_notchDepth != depth && depth > 0.0) {
        m_notchDepth = depth;
        emit notchDepthChanged(depth);
        
        // Apply to selected notch
        if (m_selectedNotch) {
            m_selectedNotch->setDepth(depth);
            m_document->setModified(true);
            if (m_canvas) m_canvas->update();
        }
    }
}

Geometry::Polyline* NotchTool::findPolylineAt(const QPointF& point, int* segmentIndex,
                                               double* position) const
{
    if (!m_document) return nullptr;

    const double tolerance = 8.0;  // pixels
    Geometry::Polyline* closestPolyline = nullptr;
    int closestSegment = -1;
    double closestPosition = 0.0;
    double closestDistance = tolerance;

    const auto& objects = m_document->objects();

    for (int i = objects.size() - 1; i >= 0; --i) {
        Geometry::GeometryObject* obj = objects[i];

        if (auto* polyline = qobject_cast<Geometry::Polyline*>(obj)) {
            QPointF closestPoint;
            int seg = polyline->findClosestSegment(point, &closestPoint);

            if (seg >= 0) {
                QPointF delta = point - closestPoint;
                double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestPolyline = polyline;
                    closestSegment = seg;

                    // Calculate position along segment
                    auto vertices = polyline->vertices();
                    int nextIdx = (seg + 1) % vertices.size();
                    QPointF p1 = vertices[seg].position;
                    QPointF p2 = vertices[nextIdx].position;
                    QPointF segVec = p2 - p1;
                    double segLength = std::sqrt(segVec.x() * segVec.x() + segVec.y() * segVec.y());
                    
                    if (segLength > 0.0001) {
                        QPointF toPoint = closestPoint - p1;
                        closestPosition = std::sqrt(toPoint.x() * toPoint.x() + toPoint.y() * toPoint.y()) / segLength;
                        closestPosition = std::max(0.0, std::min(1.0, closestPosition));
                    } else {
                        closestPosition = 0.0;
                    }
                }
            }
        }
    }

    if (segmentIndex) *segmentIndex = closestSegment;
    if (position) *position = closestPosition;

    return closestPolyline;
}

Notch* NotchTool::findNotchAt(const QPointF& point, Geometry::Polyline** polyline) const
{
    if (!m_document) return nullptr;

    const double tolerance = 10.0;  // pixels

    const auto& objects = m_document->objects();

    for (int i = objects.size() - 1; i >= 0; --i) {
        Geometry::GeometryObject* obj = objects[i];

        if (auto* poly = qobject_cast<Geometry::Polyline*>(obj)) {
            for (Notch* notch : poly->notches()) {
                QPointF notchPos = notch->getLocation();
                QPointF delta = point - notchPos;
                double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (distance <= tolerance) {
                    if (polyline) *polyline = poly;
                    return notch;
                }
            }
        }
    }

    return nullptr;
}

void NotchTool::placeNotch()
{
    if (!m_hoveredPolyline || m_hoveredSegment < 0 || !m_document) return;

    // Create new notch
    Notch* notch = new Notch(m_hoveredPolyline);
    notch->setStyle(m_notchStyle);
    notch->setDepth(m_notchDepth);
    notch->setSegmentIndex(m_hoveredSegment);
    notch->setPosition(m_hoveredPosition);

    // Use command for undo/redo
    auto* cmd = new AddNotchCommand(m_hoveredPolyline, notch);
    m_document->undoStack()->push(cmd);

    // Select the new notch
    selectNotch(notch, m_hoveredPolyline);

    showStatusMessage(QString("Notch placed on edge %1").arg(m_hoveredSegment + 1));
}

void NotchTool::deleteSelectedNotch()
{
    if (!m_selectedNotch || !m_selectedPolyline || !m_document) return;

    auto* cmd = new RemoveNotchCommand(m_selectedPolyline, m_selectedNotch);
    m_document->undoStack()->push(cmd);

    deselectNotch();
    showStatusMessage("Notch deleted");
    if (m_canvas) m_canvas->update();
}

void NotchTool::selectNotch(Notch* notch, Geometry::Polyline* polyline)
{
    m_selectedNotch = notch;
    m_selectedPolyline = polyline;
    emit notchSelected(notch);
    updateStatusMessage();
}

void NotchTool::deselectNotch()
{
    m_selectedNotch = nullptr;
    m_selectedPolyline = nullptr;
    emit notchSelected(nullptr);
    updateStatusMessage();
}

void NotchTool::showContextMenu(const QPoint& screenPos)
{
    if (!m_selectedNotch) return;

    // Update menu checkmarks based on current style
    // (simplified - would need QAction pointers for proper checkmarks)
    
    m_contextMenu->exec(screenPos);
}

void NotchTool::updateStatusMessage()
{
    QString message;
    QString styleName;
    
    switch (m_notchStyle) {
        case NotchStyle::VNotch: styleName = "V-Notch"; break;
        case NotchStyle::Slit: styleName = "Slit"; break;
        case NotchStyle::Dot: styleName = "Dot"; break;
    }

    if (m_selectedNotch) {
        message = QString("Notch selected | Delete: remove | 1/2/3: change style | Current: %1").arg(styleName);
    } else if (m_mode == Mode::PreviewPlace) {
        message = QString("Click to place %1 notch | Right-click: options").arg(styleName);
    } else {
        message = QString("Hover over edge to place notch | Style: %1 | 1=V 2=Slit 3=Dot").arg(styleName);
    }

    showStatusMessage(message);
}

void NotchTool::drawPreviewNotch(QPainter* painter) const
{
    if (!m_hoveredPolyline || m_hoveredSegment < 0) return;

    // Calculate position and normal
    auto vertices = m_hoveredPolyline->vertices();
    int nextIdx = (m_hoveredSegment + 1) % vertices.size();
    QPointF p1 = vertices[m_hoveredSegment].position;
    QPointF p2 = vertices[nextIdx].position;
    
    QPointF position = p1 + (p2 - p1) * m_hoveredPosition;
    QPointF tangent = p2 - p1;
    double len = std::sqrt(tangent.x() * tangent.x() + tangent.y() * tangent.y());
    if (len < 0.0001) return;
    tangent /= len;
    QPointF normal(-tangent.y(), tangent.x());

    // Draw preview with transparency
    painter->setOpacity(0.6);
    
    QPen pen(QColor(255, 128, 0), 2);  // Orange for preview
    painter->setPen(pen);
    
    double depth = m_notchDepth;
    
    switch (m_notchStyle) {
        case NotchStyle::VNotch: {
            QPointF left = position - tangent * depth * 0.5;
            QPointF right = position + tangent * depth * 0.5;
            QPointF tip = position + normal * depth;
            
            QPolygonF vNotch;
            vNotch << left << tip << right;
            painter->drawPolyline(vNotch);
            break;
        }
        case NotchStyle::Slit: {
            QPointF end = position + normal * depth;
            painter->drawLine(position, end);
            break;
        }
        case NotchStyle::Dot: {
            painter->setBrush(QColor(255, 128, 0, 128));
            painter->drawEllipse(position, depth / 2, depth / 2);
            break;
        }
    }
    
    painter->setOpacity(1.0);
}

void NotchTool::drawNotchHighlight(QPainter* painter, Notch* notch, 
                                    Geometry::Polyline* polyline) const
{
    if (!notch || !polyline) return;

    QPointF pos = notch->getLocation();
    
    // Draw selection highlight
    painter->setPen(QPen(QColor(0, 120, 255), 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(pos, 12, 12);
    
    // Draw corner markers
    painter->setPen(QPen(QColor(0, 120, 255), 1));
    double sz = 4;
    painter->drawLine(pos + QPointF(-12, -sz), pos + QPointF(-12, sz));
    painter->drawLine(pos + QPointF(12, -sz), pos + QPointF(12, sz));
    painter->drawLine(pos + QPointF(-sz, -12), pos + QPointF(sz, -12));
    painter->drawLine(pos + QPointF(-sz, 12), pos + QPointF(sz, 12));
}

void NotchTool::onStyleVNotch()
{
    setNotchStyle(NotchStyle::VNotch);
}

void NotchTool::onStyleSlit()
{
    setNotchStyle(NotchStyle::Slit);
}

void NotchTool::onStyleDot()
{
    setNotchStyle(NotchStyle::Dot);
}

void NotchTool::onDeleteNotch()
{
    deleteSelectedNotch();
}

} // namespace Tools
} // namespace PatternCAD
