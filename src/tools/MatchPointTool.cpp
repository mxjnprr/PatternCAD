/**
 * MatchPointTool.cpp
 *
 * Implementation of MatchPointTool for placing and linking match points
 */

#include "MatchPointTool.h"
#include "geometry/GeometryObject.h"
#include "geometry/Polyline.h"
#include "geometry/MatchPoint.h"
#include "core/Document.h"
#include "core/Commands.h"
#include "ui/Canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <cmath>

namespace PatternCAD {
namespace Tools {

MatchPointTool::MatchPointTool(QObject* parent)
    : Tool(parent)
    , m_mode(Mode::Idle)
    , m_defaultLabel("A")
    , m_labelCounter(0)
    , m_hoveredPolyline(nullptr)
    , m_hoveredSegment(-1)
    , m_hoveredPosition(0.0)
    , m_selectedMatchPoint(nullptr)
    , m_selectedPolyline(nullptr)
    , m_dragMatchPoint(nullptr)
    , m_dragPolyline(nullptr)
    , m_linkSourcePoint(nullptr)
    , m_linkSourcePolyline(nullptr)
    , m_contextMenu(nullptr)
{
    // Create context menu
    m_contextMenu = new QMenu();
    
    QAction* editLabelAction = m_contextMenu->addAction("Edit Label...");
    m_contextMenu->addSeparator();
    QAction* linkAction = m_contextMenu->addAction("Link to...");
    QAction* unlinkAction = m_contextMenu->addAction("Unlink All");
    m_contextMenu->addSeparator();
    QAction* deleteAction = m_contextMenu->addAction("Delete");
    
    connect(editLabelAction, &QAction::triggered, this, &MatchPointTool::onEditLabel);
    connect(linkAction, &QAction::triggered, this, &MatchPointTool::onLinkMatchPoint);
    connect(unlinkAction, &QAction::triggered, this, &MatchPointTool::onUnlinkMatchPoint);
    connect(deleteAction, &QAction::triggered, this, &MatchPointTool::onDeleteMatchPoint);
}

MatchPointTool::~MatchPointTool()
{
    delete m_contextMenu;
}

QString MatchPointTool::name() const
{
    return "MatchPoint";
}

QString MatchPointTool::description() const
{
    return "Place match points on pattern edges (M)";
}

QCursor MatchPointTool::cursor() const
{
    if (m_mode == Mode::Linking) {
        return Qt::PointingHandCursor;
    }
    return Qt::CrossCursor;
}

void MatchPointTool::activate()
{
    Tool::activate();
    reset();
    updateStatusMessage();
}

void MatchPointTool::deactivate()
{
    reset();
    Tool::deactivate();
}

void MatchPointTool::reset()
{
    m_mode = Mode::Idle;
    m_hoveredPolyline = nullptr;
    m_hoveredSegment = -1;
    m_hoveredPosition = 0.0;
    m_selectedMatchPoint = nullptr;
    m_selectedPolyline = nullptr;
    m_dragMatchPoint = nullptr;
    m_dragPolyline = nullptr;
    m_linkSourcePoint = nullptr;
    m_linkSourcePolyline = nullptr;
    updateStatusMessage();
}

void MatchPointTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        // Check if clicking on an existing match point
        Geometry::Polyline* mpPolyline = nullptr;
        MatchPoint* clickedMP = findMatchPointAt(scenePos, &mpPolyline);
        
        if (m_mode == Mode::Linking) {
            // Complete linking if clicking another match point
            if (clickedMP && clickedMP != m_linkSourcePoint) {
                completeLinking(clickedMP, mpPolyline);
            } else {
                cancelLinking();
            }
        } else if (event->modifiers() & Qt::ShiftModifier && clickedMP) {
            // Shift+click starts linking mode
            selectMatchPoint(clickedMP, mpPolyline);
            startLinking();
        } else if (clickedMP) {
            // Select the match point for dragging or editing
            selectMatchPoint(clickedMP, mpPolyline);
            m_mode = Mode::Dragging;
            m_dragMatchPoint = clickedMP;
            m_dragPolyline = mpPolyline;
        } else if (m_hoveredPolyline && m_hoveredSegment >= 0) {
            // Place a new match point
            placeMatchPoint();
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_mode == Mode::Linking) {
            cancelLinking();
        } else {
            // Right-click context menu
            Geometry::Polyline* mpPolyline = nullptr;
            MatchPoint* clickedMP = findMatchPointAt(scenePos, &mpPolyline);
            
            if (clickedMP) {
                selectMatchPoint(clickedMP, mpPolyline);
                showContextMenu(event->globalPosition().toPoint());
            } else {
                deselectMatchPoint();
            }
        }
    }

    if (m_canvas) m_canvas->update();
}

void MatchPointTool::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());

    if (m_mode == Mode::Dragging && m_dragMatchPoint && m_dragPolyline) {
        // Update match point position while dragging
        int segmentIndex = -1;
        double position = 0.0;
        Geometry::Polyline* poly = findPolylineAt(scenePos, &segmentIndex, &position);
        
        if (poly == m_dragPolyline && segmentIndex >= 0) {
            // Update match point position on same polyline
            m_dragMatchPoint->setSegmentIndex(segmentIndex);
            m_dragMatchPoint->setSegmentPosition(position);
            m_document->setModified(true);
        }
    } else if (m_mode == Mode::Linking) {
        // Update hover for linking target
        m_hoveredPoint = scenePos;
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

void MatchPointTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_mode == Mode::Dragging) {
        // Finish dragging
        m_mode = Mode::Idle;
        m_dragMatchPoint = nullptr;
        m_dragPolyline = nullptr;
        updateStatusMessage();
        if (m_canvas) m_canvas->update();
    }
}

void MatchPointTool::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());
    
    // Double-click to edit label
    Geometry::Polyline* mpPolyline = nullptr;
    MatchPoint* clickedMP = findMatchPointAt(scenePos, &mpPolyline);
    
    if (clickedMP) {
        selectMatchPoint(clickedMP, mpPolyline);
        editLabel();
    }
}

void MatchPointTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        if (m_mode == Mode::Linking) {
            cancelLinking();
        } else {
            deselectMatchPoint();
            reset();
        }
        if (m_canvas) m_canvas->update();
    } else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        deleteSelectedMatchPoint();
    } else if (event->key() == Qt::Key_L && m_selectedMatchPoint) {
        startLinking();
    } else if (event->key() == Qt::Key_E && m_selectedMatchPoint) {
        editLabel();
    }
}

void MatchPointTool::drawOverlay(QPainter* painter)
{
    if (!painter || !m_document) return;

    painter->save();

    // Draw preview match point at hover position
    if (m_mode == Mode::PreviewPlace && m_hoveredPolyline && m_hoveredSegment >= 0) {
        drawPreviewMatchPoint(painter);
    }

    // Highlight selected match point
    if (m_selectedMatchPoint && m_selectedPolyline) {
        drawMatchPointHighlight(painter, m_selectedMatchPoint, m_selectedPolyline);
    }

    // Draw linking line
    if (m_mode == Mode::Linking && m_linkSourcePoint && m_linkSourcePolyline) {
        drawLinkingLine(painter);
    }

    painter->restore();
}

void MatchPointTool::setDefaultLabel(const QString& label)
{
    m_defaultLabel = label;
}

Geometry::Polyline* MatchPointTool::findPolylineAt(const QPointF& point, int* segmentIndex,
                                                    double* position) const
{
    if (!m_document) return nullptr;

    const double tolerance = 8.0;
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

MatchPoint* MatchPointTool::findMatchPointAt(const QPointF& point, Geometry::Polyline** polyline) const
{
    if (!m_document) return nullptr;

    const double tolerance = 12.0;

    const auto& objects = m_document->objects();

    for (int i = objects.size() - 1; i >= 0; --i) {
        Geometry::GeometryObject* obj = objects[i];

        if (auto* poly = qobject_cast<Geometry::Polyline*>(obj)) {
            for (MatchPoint* mp : poly->matchPoints()) {
                QPointF mpPos = mp->position();
                QPointF delta = point - mpPos;
                double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (distance <= tolerance) {
                    if (polyline) *polyline = poly;
                    return mp;
                }
            }
        }
    }

    return nullptr;
}

void MatchPointTool::placeMatchPoint()
{
    if (!m_hoveredPolyline || m_hoveredSegment < 0 || !m_document) return;

    // Create new match point
    MatchPoint* mp = new MatchPoint(m_hoveredPolyline);
    mp->setLabel(generateNextLabel());
    mp->setSegmentIndex(m_hoveredSegment);
    mp->setSegmentPosition(m_hoveredPosition);

    // Use command for undo/redo
    auto* cmd = new AddMatchPointCommand(m_hoveredPolyline, mp);
    m_document->undoStack()->push(cmd);

    // Select the new match point
    selectMatchPoint(mp, m_hoveredPolyline);

    showStatusMessage(QString("Match point '%1' placed").arg(mp->label()));
}

void MatchPointTool::deleteSelectedMatchPoint()
{
    if (!m_selectedMatchPoint || !m_selectedPolyline || !m_document) return;

    QString label = m_selectedMatchPoint->label();
    
    auto* cmd = new RemoveMatchPointCommand(m_selectedPolyline, m_selectedMatchPoint);
    m_document->undoStack()->push(cmd);

    deselectMatchPoint();
    showStatusMessage(QString("Match point '%1' deleted").arg(label));
    if (m_canvas) m_canvas->update();
}

void MatchPointTool::selectMatchPoint(MatchPoint* matchPoint, Geometry::Polyline* polyline)
{
    m_selectedMatchPoint = matchPoint;
    m_selectedPolyline = polyline;
    emit matchPointSelected(matchPoint);
    updateStatusMessage();
}

void MatchPointTool::deselectMatchPoint()
{
    m_selectedMatchPoint = nullptr;
    m_selectedPolyline = nullptr;
    emit matchPointSelected(nullptr);
    updateStatusMessage();
}

void MatchPointTool::startLinking()
{
    if (!m_selectedMatchPoint || !m_selectedPolyline) return;

    m_mode = Mode::Linking;
    m_linkSourcePoint = m_selectedMatchPoint;
    m_linkSourcePolyline = m_selectedPolyline;
    
    emit linkingStarted(m_linkSourcePoint);
    updateStatusMessage();
    if (m_canvas) m_canvas->update();
}

void MatchPointTool::completeLinking(MatchPoint* target, Geometry::Polyline* targetPolyline)
{
    if (!m_linkSourcePoint || !target || target == m_linkSourcePoint) {
        cancelLinking();
        return;
    }

    // Check if already linked
    if (m_linkSourcePoint->isLinkedTo(target)) {
        // Unlink instead
        auto* cmd = new LinkMatchPointsCommand(m_linkSourcePoint, target, false);
        m_document->undoStack()->push(cmd);
        showStatusMessage(QString("Unlinked '%1' from '%2'")
            .arg(m_linkSourcePoint->label()).arg(target->label()));
    } else {
        // Create link
        auto* cmd = new LinkMatchPointsCommand(m_linkSourcePoint, target, true);
        m_document->undoStack()->push(cmd);
        showStatusMessage(QString("Linked '%1' to '%2'")
            .arg(m_linkSourcePoint->label()).arg(target->label()));
    }

    emit linkingCompleted(m_linkSourcePoint, target);
    
    // Reset linking mode
    m_mode = Mode::Idle;
    m_linkSourcePoint = nullptr;
    m_linkSourcePolyline = nullptr;
    
    if (m_canvas) m_canvas->update();
}

void MatchPointTool::cancelLinking()
{
    m_mode = Mode::Idle;
    m_linkSourcePoint = nullptr;
    m_linkSourcePolyline = nullptr;
    updateStatusMessage();
    if (m_canvas) m_canvas->update();
}

void MatchPointTool::editLabel()
{
    if (!m_selectedMatchPoint) return;

    bool ok;
    QString newLabel = QInputDialog::getText(
        nullptr,
        "Edit Match Point Label",
        "Label:",
        QLineEdit::Normal,
        m_selectedMatchPoint->label(),
        &ok
    );

    if (ok && !newLabel.isEmpty()) {
        auto* cmd = new ModifyMatchPointCommand(
            m_selectedMatchPoint,
            newLabel,
            m_selectedMatchPoint->segmentIndex(),
            m_selectedMatchPoint->segmentPosition()
        );
        m_document->undoStack()->push(cmd);
        showStatusMessage(QString("Label changed to '%1'").arg(newLabel));
        if (m_canvas) m_canvas->update();
    }
}

void MatchPointTool::showContextMenu(const QPoint& screenPos)
{
    if (!m_selectedMatchPoint) return;
    m_contextMenu->exec(screenPos);
}

void MatchPointTool::updateStatusMessage()
{
    QString message;

    if (m_mode == Mode::Linking) {
        message = QString("Click on another match point to link with '%1' | Esc to cancel")
            .arg(m_linkSourcePoint ? m_linkSourcePoint->label() : "");
    } else if (m_selectedMatchPoint) {
        int linkCount = m_selectedMatchPoint->linkedPoints().size();
        QString linkInfo = linkCount > 0 ? QString(" | %1 link(s)").arg(linkCount) : "";
        message = QString("Match point '%1'%2 | Delete: remove | L: link | E: edit label | Shift+click: link")
            .arg(m_selectedMatchPoint->label()).arg(linkInfo);
    } else if (m_mode == Mode::PreviewPlace) {
        message = QString("Click to place match point '%1' | Double-click to edit label")
            .arg(peekNextLabel());
    } else {
        message = "Hover over edge to place match point | Shift+click to link points";
    }

    showStatusMessage(message);
}

QString MatchPointTool::generateNextLabel()
{
    // Generate labels: A, B, C, ... Z, AA, AB, ...
    QString label = peekNextLabel();
    m_labelCounter++;
    return label;
}

QString MatchPointTool::peekNextLabel() const
{
    // Generate label without incrementing counter (for preview)
    QString label;
    int n = m_labelCounter;
    
    do {
        label.prepend(QChar('A' + (n % 26)));
        n = n / 26 - 1;
    } while (n >= 0);
    
    return label;
}

void MatchPointTool::drawPreviewMatchPoint(QPainter* painter) const
{
    if (!m_hoveredPolyline || m_hoveredSegment < 0) return;

    painter->setOpacity(0.6);
    
    QPen pen(QColor(128, 0, 255), 2);  // Purple for preview
    painter->setPen(pen);
    painter->setBrush(QColor(128, 0, 255, 80));
    
    // Draw cross with circle
    double size = 8.0;
    painter->drawEllipse(m_hoveredPoint, size, size);
    painter->drawLine(m_hoveredPoint + QPointF(-size * 1.5, 0), 
                      m_hoveredPoint + QPointF(size * 1.5, 0));
    painter->drawLine(m_hoveredPoint + QPointF(0, -size * 1.5), 
                      m_hoveredPoint + QPointF(0, size * 1.5));
    
    // Draw label preview
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(10);
    painter->setFont(font);
    painter->drawText(m_hoveredPoint + QPointF(12, -8), peekNextLabel());
    
    painter->setOpacity(1.0);
}

void MatchPointTool::drawMatchPointHighlight(QPainter* painter, MatchPoint* matchPoint, 
                                              Geometry::Polyline* polyline) const
{
    if (!matchPoint || !polyline) return;

    QPointF pos = matchPoint->position();
    
    // Draw selection highlight
    painter->setPen(QPen(QColor(0, 120, 255), 3));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(pos, 14, 14);
    
    // Draw animated dashed circle
    QPen dashPen(QColor(0, 120, 255), 1, Qt::DashLine);
    painter->setPen(dashPen);
    painter->drawEllipse(pos, 18, 18);
}

void MatchPointTool::drawLinkingLine(QPainter* painter) const
{
    if (!m_linkSourcePoint || !m_linkSourcePolyline) return;

    QPointF sourcePos = m_linkSourcePoint->position();
    
    // Draw dashed line from source to current mouse position
    QPen pen(QColor(0, 200, 100), 2, Qt::DashLine);
    painter->setPen(pen);
    painter->drawLine(sourcePos, m_hoveredPoint);
    
    // Draw arrow at end
    QPointF dir = m_hoveredPoint - sourcePos;
    double len = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
    if (len > 10) {
        dir /= len;
        QPointF perp(-dir.y(), dir.x());
        QPointF arrowTip = m_hoveredPoint;
        QPointF arrowLeft = arrowTip - dir * 10 + perp * 5;
        QPointF arrowRight = arrowTip - dir * 10 - perp * 5;
        
        painter->setBrush(QColor(0, 200, 100));
        QPolygonF arrow;
        arrow << arrowTip << arrowLeft << arrowRight;
        painter->drawPolygon(arrow);
    }
}

void MatchPointTool::onEditLabel()
{
    editLabel();
}

void MatchPointTool::onLinkMatchPoint()
{
    startLinking();
}

void MatchPointTool::onUnlinkMatchPoint()
{
    if (!m_selectedMatchPoint || !m_document) return;

    // Unlink all linked points
    auto linkedPoints = m_selectedMatchPoint->linkedPoints();
    for (MatchPoint* other : linkedPoints) {
        auto* cmd = new LinkMatchPointsCommand(m_selectedMatchPoint, other, false);
        m_document->undoStack()->push(cmd);
    }
    
    showStatusMessage(QString("Unlinked '%1' from all points").arg(m_selectedMatchPoint->label()));
    if (m_canvas) m_canvas->update();
}

void MatchPointTool::onDeleteMatchPoint()
{
    deleteSelectedMatchPoint();
}

} // namespace Tools
} // namespace PatternCAD
