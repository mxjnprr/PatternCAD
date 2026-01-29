/**
 * SelectTool.cpp
 *
 * Implementation of SelectTool
 */

#include "SelectTool.h"
#include "ui/Canvas.h"
#include "core/Document.h"
#include "core/Commands.h"
#include "core/Units.h"
#include "geometry/GeometryObject.h"
#include "geometry/Line.h"
#include "geometry/Polyline.h"
#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QWidget>
#include <QFontMetrics>
#include <cmath>

namespace PatternCAD {
namespace Tools {

SelectTool::SelectTool(QObject* parent)
    : Tool(parent)
    , m_mode(SelectMode::None)
    , m_multiSelect(false)
    , m_hoveredObject(nullptr)
    , m_selectedVertexObject(nullptr)
    , m_selectedVertexIndex(-1)
    , m_hoveredVertexIndex(-1)
    , m_selectedSegmentObject(nullptr)
    , m_selectedSegmentIndex(-1)
    , m_hoveredSegmentIndex(-1)
    , m_selectedHandleObject(nullptr)
    , m_selectedHandleVertexIndex(-1)
    , m_selectedHandleSide(0)
    , m_hoveredHandleVertexIndex(-1)
    , m_hoveredHandleSide(0)
    , m_handleStartTension(0.5)
    , m_constrainedSegmentIndex(-1)
    , m_constrainedLength(0.0)
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
    if (m_mode == SelectMode::Dragging) {
        return QCursor(Qt::ClosedHandCursor);
    } else if (m_hoveredObject) {
        return QCursor(Qt::OpenHandCursor);
    }
    return QCursor(Qt::ArrowCursor);
}

void SelectTool::activate()
{
    Tool::activate();
    m_mode = SelectMode::None;
    m_hoveredObject = nullptr;
    m_selectedVertexObject = nullptr;
    m_selectedVertexIndex = -1;
    m_hoveredVertexIndex = -1;
    updateStatusMessage();
}

void SelectTool::reset()
{
    Tool::reset();
    m_mode = SelectMode::None;
    m_multiSelect = false;
    m_hoveredObject = nullptr;
}

void SelectTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) {
        return;
    }

    m_currentPoint = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        if (m_mode == SelectMode::Dragging) {
            // Place the dragged object at current position
            finishMove();
            m_mode = SelectMode::None;
            showStatusMessage("Object placed");
        } else {
            // Check if clicking on a curve handle first
            Geometry::GeometryObject* handleObject = nullptr;
            int handleSide = 0;
            int handleVertexIndex = findHandleAt(m_currentPoint, &handleObject, &handleSide);

            if (handleVertexIndex >= 0) {
                // Check if layer is locked
                if (m_document->isLayerLocked(handleObject->layer())) {
                    showStatusMessage("Cannot modify object on locked layer");
                    event->accept();
                    return;
                }

                // Start dragging handle
                m_selectedHandleObject = handleObject;
                m_selectedHandleVertexIndex = handleVertexIndex;
                m_selectedHandleSide = handleSide;
                m_mode = SelectMode::DraggingHandle;

                // Also select the vertex
                m_selectedVertexObject = handleObject;
                m_selectedVertexIndex = handleVertexIndex;

                // Save initial tangent and tension for undo
                if (auto* polyline = dynamic_cast<Geometry::Polyline*>(handleObject)) {
                    auto vertices = polyline->vertices();
                    m_handleStartTangent = vertices[handleVertexIndex].tangent;
                    // Store the tension for the side we're dragging
                    m_handleStartTension = (handleSide < 0)
                        ? vertices[handleVertexIndex].incomingTension
                        : vertices[handleVertexIndex].outgoingTension;
                }

                showStatusMessage(QString("Dragging curve handle | Esc: cancel"));
            } else {
                // Check if clicking on a vertex
                Geometry::GeometryObject* vertexObject = nullptr;
                int vertexIndex = findVertexAt(m_currentPoint, &vertexObject);

                if (vertexIndex >= 0) {
                // Check if layer is locked
                if (m_document->isLayerLocked(vertexObject->layer())) {
                    showStatusMessage("Cannot modify object on locked layer");
                    event->accept();
                    return;
                }

                // Select vertex (but don't start moving yet)
                // Check if this is the same vertex we already had selected
                bool sameVertex = (m_selectedVertexObject == vertexObject && m_selectedVertexIndex == vertexIndex);

                m_selectedVertexObject = vertexObject;
                m_selectedVertexIndex = vertexIndex;
                m_mode = SelectMode::VertexSelected;

                // Save initial vertex position for potential undo
                if (auto* polyline = dynamic_cast<Geometry::Polyline*>(vertexObject)) {
                    m_vertexStartPosition = polyline->vertexAt(vertexIndex).position;
                } else if (auto* line = dynamic_cast<Geometry::Line*>(vertexObject)) {
                    m_vertexStartPosition = (vertexIndex == 0) ? line->start() : line->end();
                }

                // Only reset constraint if clicking on a different vertex
                if (!sameVertex) {
                    m_constrainedSegmentIndex = -1;
                }

                showStatusMessage(QString("Vertex %1 selected | G: grab | L: lock | T: toggle type | Tab: dimension | Del: delete")
                    .arg(vertexIndex + 1));
            } else {
                // Check if clicking on a segment
                Geometry::GeometryObject* segmentObject = nullptr;
                int segmentIndex = findSegmentAt(m_currentPoint, &segmentObject);

                if (segmentIndex >= 0) {
                    // Check if layer is locked
                    if (m_document->isLayerLocked(segmentObject->layer())) {
                        showStatusMessage("Cannot modify object on locked layer");
                        event->accept();
                        return;
                    }

                    // Check if Shift is pressed → toggle segment selection for multi-lock
                    if (event->modifiers() & Qt::ShiftModifier) {
                        toggleSegmentSelection(segmentObject, segmentIndex);
                    } else {
                        // Regular segment selection
                        m_selectedSegmentObject = segmentObject;
                        m_selectedSegmentIndex = segmentIndex;
                        m_mode = SelectMode::None; // Reset mode
                        m_selectedVertexObject = nullptr;
                        m_selectedVertexIndex = -1;

                        // Calculate current segment length for display
                        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(segmentObject)) {
                            double length = polyline->calculateSegmentLength(segmentIndex);
                            showStatusMessage(QString("Segment %1 selected (%2) | Tab: change dimension")
                                .arg(segmentIndex + 1)
                                .arg(Units::formatLength(length, 2)));
                        }
                    }
                } else {
                    // Start potential rectangular selection or object click
                    m_startPoint = m_currentPoint;
                    m_selectionRect = QRectF();
                    m_multiSelect = (event->modifiers() & Qt::ControlModifier);
                }
            }
            }  // Close the else block for handle check
        }
    } else if (event->button() == Qt::RightButton) {
        // Right-click handling moved to mouseReleaseEvent to avoid double menu
        event->accept();
        return;
    }

    event->accept();
}

void SelectTool::mouseMoveEvent(QMouseEvent* event)
{
    QPointF oldPoint = m_currentPoint;
    m_currentPoint = mapToScene(event->pos());

    if (m_mode == SelectMode::DraggingHandle) {
        // Move curve handle and update tangent
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedHandleObject)) {
            auto vertices = polyline->vertices();
            int n = vertices.size();

            if (m_selectedHandleVertexIndex >= 0 && m_selectedHandleVertexIndex < n) {
                Geometry::PolylineVertex vertex = vertices[m_selectedHandleVertexIndex];
                QPointF vertexPos = vertex.position;

                // Determine which segment is affected by this handle
                int affectedSegment = (m_selectedHandleSide < 0)
                    ? (m_selectedHandleVertexIndex - 1 + n) % n  // Incoming handle affects previous segment
                    : m_selectedHandleVertexIndex;                // Outgoing handle affects this segment

                // Check if this vertex is adjacent to a locked segment
                // If any segment adjacent to this vertex is locked, rotation is locked for BOTH handles
                int prevSegment = (m_selectedHandleVertexIndex - 1 + n) % n;
                int nextSegment = m_selectedHandleVertexIndex;

                // Check legacy single-segment lock
                bool isVertexAdjacentToLockedSegment = (m_constrainedSegmentIndex >= 0) &&
                    (m_constrainedSegmentIndex == prevSegment || m_constrainedSegmentIndex == nextSegment);

                // Check multi-segment locks
                SegmentKey prevKey{polyline, prevSegment};
                SegmentKey nextKey{polyline, nextSegment};
                bool isVertexAdjacentToMultiLockedSegment = m_lockedSegments.contains(prevKey) ||
                                                             m_lockedSegments.contains(nextKey);

                isVertexAdjacentToLockedSegment = isVertexAdjacentToLockedSegment || isVertexAdjacentToMultiLockedSegment;

                // Check if the specific segment affected by this handle is locked (for length constraint)
                SegmentKey affectedKey{polyline, affectedSegment};
                bool isAffectedSegmentLocked = (m_constrainedSegmentIndex >= 0 && affectedSegment == m_constrainedSegmentIndex) ||
                                                m_lockedSegments.contains(affectedKey);

                // Calculate new tangent direction (from vertex to handle)
                QPointF handlePos = m_currentPoint;
                QPointF tangentDir;
                double tangentLen;

                if (isVertexAdjacentToLockedSegment) {
                    // Vertex is adjacent to a locked segment - keep tangent direction, only adjust length along that direction
                    // Get current tangent direction
                    QPointF currentTangent = vertex.tangent;
                    if (currentTangent == QPointF()) {
                        // No explicit tangent yet, calculate from Catmull-Rom
                        int prevIdx = (m_selectedHandleVertexIndex - 1 + n) % n;
                        int nextIdx = (m_selectedHandleVertexIndex + 1) % n;
                        QPointF p0 = vertices[prevIdx].position;
                        QPointF p2 = vertices[nextIdx].position;
                        currentTangent = (p2 - p0);
                        double len = std::sqrt(currentTangent.x() * currentTangent.x() + currentTangent.y() * currentTangent.y());
                        if (len > 0.001) {
                            currentTangent /= len;
                        }
                    }

                    // Tangent direction stays the same (in outgoing direction)
                    tangentDir = currentTangent;

                    // Project mouse movement onto the tangent direction
                    QPointF mouseDir = handlePos - vertexPos;
                    double projection = mouseDir.x() * tangentDir.x() + mouseDir.y() * tangentDir.y();

                    // If this is incoming handle, we need opposite direction
                    if (m_selectedHandleSide < 0) {
                        projection = -projection;
                    }

                    // Tangent length is the projection (can be negative if pulling opposite direction)
                    tangentLen = std::abs(projection);
                } else {
                    // Segment is not locked - free movement
                    QPointF mouseDir = handlePos - vertexPos;
                    tangentLen = std::sqrt(mouseDir.x() * mouseDir.x() + mouseDir.y() * mouseDir.y());

                    if (tangentLen > 0.001) {
                        tangentDir = mouseDir / tangentLen;
                    } else {
                        tangentDir = QPointF(1, 0);
                    }

                    // Handle direction depends on which side
                    if (m_selectedHandleSide < 0) {
                        // Incoming handle - flip tangent direction
                        tangentDir = -tangentDir;
                    }
                }

                if (tangentLen > 0.001) {
                    // Calculate reference distance (distance between neighbors)
                    int prevIdx = (m_selectedHandleVertexIndex - 1 + n) % n;
                    int nextIdx = (m_selectedHandleVertexIndex + 1) % n;
                    QPointF p0 = vertices[prevIdx].position;
                    QPointF p2 = vertices[nextIdx].position;
                    double refDist = std::sqrt((p2 - p0).x() * (p2 - p0).x() + (p2 - p0).y() * (p2 - p0).y());

                    // Calculate initial tension based on handle distance
                    double expectedDist = refDist / 6.0;
                    double tension = (expectedDist > 0.001) ? (tangentLen / expectedDist) : 0.5;
                    // No limit on tension - user has full control

                    // Update vertex tangent (always store in outgoing direction)
                    vertex.tangent = tangentDir;

                    // Apply length constraint if active
                    if (isAffectedSegmentLocked) {
                        // This handle affects the locked segment
                        // We need to find the tension that maintains the locked length
                        // Keep the tangent direction, but adjust tension

                        // Get the target locked length
                        double targetLength = m_constrainedLength;  // Legacy lock default
                        if (m_lockedSegments.contains(affectedKey)) {
                            targetLength = m_lockedLengths[affectedKey];
                        }

                        // Binary search for the correct tension
                        double minTension = 0.01;
                        double maxTension = 20.0;
                        double bestTension = tension;

                        const int maxIterations = 20;
                        for (int iter = 0; iter < maxIterations; ++iter) {
                            double testTension = (minTension + maxTension) / 2.0;

                            // Apply test tension
                            if (m_selectedHandleSide < 0) {
                                vertex.incomingTension = testTension;
                            } else {
                                vertex.outgoingTension = testTension;
                            }
                            vertices[m_selectedHandleVertexIndex] = vertex;
                            polyline->setVertices(vertices);

                            // Calculate resulting length
                            double currentLength = polyline->calculateSegmentLength(affectedSegment);
                            double error = currentLength - targetLength;

                            // Check convergence
                            if (std::abs(error) < 0.05) {
                                bestTension = testTension;
                                break;
                            }

                            // Adjust search range
                            if (currentLength > targetLength) {
                                // Curve too long, reduce tension
                                maxTension = testTension;
                            } else {
                                // Curve too short, increase tension
                                minTension = testTension;
                            }

                            bestTension = testTension;
                        }

                        // Apply final tension
                        if (m_selectedHandleSide < 0) {
                            vertex.incomingTension = bestTension;
                        } else {
                            vertex.outgoingTension = bestTension;
                        }
                    } else {
                        // No constraint - use calculated tension
                        if (m_selectedHandleSide < 0) {
                            vertex.incomingTension = tension;
                        } else {
                            vertex.outgoingTension = tension;
                        }
                    }

                    vertices[m_selectedHandleVertexIndex] = vertex;
                    polyline->setVertices(vertices);

                    if (m_canvas) {
                        m_canvas->viewport()->update();
                    }
                }
            }
        }
    } else if (m_mode == SelectMode::DraggingVertex) {
        // Move vertex with optional length constraint
        QPointF newPosition = m_currentPoint;

        // Apply length constraint if active
        if (m_constrainedSegmentIndex >= 0) {
            if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
                auto vertices = polyline->vertices();
                int n = vertices.size();

                // Determine which endpoint of the constrained segment is the vertex being moved
                bool isSegmentStart = (m_constrainedSegmentIndex == m_selectedVertexIndex);
                bool isSegmentEnd = ((m_constrainedSegmentIndex + 1) % n == m_selectedVertexIndex);

                if (isSegmentStart || isSegmentEnd) {
                    // Get the other (fixed) endpoint
                    QPointF fixedPoint;
                    if (isSegmentStart) {
                        int endIdx = (m_constrainedSegmentIndex + 1) % n;
                        fixedPoint = vertices[endIdx].position;
                    } else {
                        fixedPoint = vertices[m_constrainedSegmentIndex].position;
                    }

                    // Use iterative adjustment to maintain curve length
                    // Start with the desired position
                    QPointF testPosition = m_currentPoint;

                    // Iteratively adjust to maintain the constrained length
                    const int maxIterations = 8;
                    for (int iter = 0; iter < maxIterations; ++iter) {
                        // Calculate the resulting segment length with test position
                        double currentLength = polyline->calculateSegmentLength(
                            m_constrainedSegmentIndex,
                            m_selectedVertexIndex,
                            testPosition
                        );

                        // Check if we're close enough
                        double error = currentLength - m_constrainedLength;
                        if (std::abs(error) < 0.1) {
                            // Close enough
                            break;
                        }

                        // Adjust position to correct the error
                        // Move closer or farther from the fixed point
                        QPointF dir = testPosition - fixedPoint;
                        double dist = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());

                        if (dist > 0.001) {
                            dir /= dist;
                            // Adjust the distance based on error (use a factor to avoid overshooting)
                            double adjustment = -error * 0.7;
                            testPosition = fixedPoint + dir * (dist + adjustment);
                        } else {
                            // If too close, move away in the direction of the mouse
                            QPointF mouseDir = m_currentPoint - fixedPoint;
                            double mouseDist = std::sqrt(mouseDir.x() * mouseDir.x() + mouseDir.y() * mouseDir.y());
                            if (mouseDist > 0.001) {
                                testPosition = fixedPoint + (mouseDir / mouseDist) * m_constrainedLength;
                            }
                            break;
                        }
                    }

                    newPosition = testPosition;
                }
                // If vertex is not part of constrained segment, no constraint applies
            }
        }

        // Apply multi-segment lock constraints
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            newPosition = constrainVertexWithLockedSegments(newPosition, polyline, m_selectedVertexIndex);
        }

        // Apply the new position (but don't notify document yet - wait for mouseRelease)
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            Geometry::PolylineVertex vertex = polyline->vertexAt(m_selectedVertexIndex);
            vertex.position = newPosition;
            polyline->updateVertex(m_selectedVertexIndex, newPosition);
        } else if (auto* line = dynamic_cast<Geometry::Line*>(m_selectedVertexObject)) {
            // For lines, move start or end point
            if (m_selectedVertexIndex == 0) {
                line->setStart(newPosition);
            } else {
                line->setEnd(newPosition);
            }
        }

        m_lastPoint = newPosition;
    } else if (m_mode == SelectMode::Dragging) {
        // Move selected objects to follow cursor
        QPointF delta = m_currentPoint - oldPoint;
        updateMove(delta);
        m_lastPoint = m_currentPoint;
    } else if (event->buttons() & Qt::LeftButton) {
        // Check if dragging for selection box
        QPointF delta = m_currentPoint - m_startPoint;
        double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (distance > 5.0) {  // Threshold to distinguish click from drag
            m_mode = SelectMode::Selecting;
            m_selectionRect = QRectF(m_startPoint, m_currentPoint).normalized();
            showStatusMessage("Drag to select objects in box");
        }
    } else {
        // Update hovered handle first (takes priority over vertex)
        Geometry::GeometryObject* handleObject = nullptr;
        int handleSide = 0;
        m_hoveredHandleVertexIndex = findHandleAt(m_currentPoint, &handleObject, &handleSide);
        m_hoveredHandleSide = handleSide;

        if (m_hoveredHandleVertexIndex >= 0) {
            QString handleName = (handleSide < 0) ? "incoming" : "outgoing";
            showStatusMessage(QString("Curve handle (%1) | Click and drag to adjust curve")
                .arg(handleName));
            if (m_canvas) {
                m_canvas->viewport()->update();
            }
        } else {
            // Update hovered vertex
            Geometry::GeometryObject* vertexObject = nullptr;
            m_hoveredVertexIndex = findVertexAt(m_currentPoint, &vertexObject);

            if (m_hoveredVertexIndex >= 0) {
                showStatusMessage(QString("Vertex %1 | Click: move | L: lock segment | T: toggle type | Tab: dimension | Del: delete")
                    .arg(m_hoveredVertexIndex + 1));
            } else {
            // Update hovered object for visual feedback
            Geometry::GeometryObject* newHovered = findObjectAt(m_currentPoint);
            if (newHovered != m_hoveredObject) {
                m_hoveredObject = newHovered;
                if (m_hoveredObject) {
                    showStatusMessage(QString("Click to select %1 (Ctrl+Click for multi-select, Space to pick)")
                        .arg(m_hoveredObject->typeName()));
                } else {
                    showStatusMessage("Click object or drag box to select");
                }
            }
            }  // Close the else block for m_hoveredVertexIndex
        }  // Close the else block for m_hoveredHandleVertexIndex
    }

    event->accept();
}

void SelectTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_document) {
        return;
    }

    if (event->button() == Qt::RightButton) {
        // Right click: check if clicking on a locked segment
        Geometry::GeometryObject* segmentObject = nullptr;
        int segmentIndex = findSegmentAt(m_currentPoint, &segmentObject);

        if (segmentIndex >= 0 && isSegmentLocked(segmentObject, segmentIndex)) {
            // Show context menu for locked segment
            QMenu menu;
            QAction* unlockAction = menu.addAction("🔓 Unlock Segment");
            QAction* unlockAllAction = menu.addAction("🔓 Unlock All Segments");

            QAction* selectedAction = menu.exec(event->globalPosition().toPoint());
            if (selectedAction == unlockAction) {
                unlockSegment(segmentObject, segmentIndex);
            } else if (selectedAction == unlockAllAction) {
                unlockAllSegments();
            }
        } else {
            // Show regular context menu
            showContextMenu(event->globalPosition().toPoint());
        }

        event->accept();
        return;
    }

    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (m_mode == SelectMode::DraggingHandle) {
        // Finish handle drag - create undo command
        if (m_document && m_selectedHandleObject) {
            if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedHandleObject)) {
                auto vertices = polyline->vertices();
                if (m_selectedHandleVertexIndex >= 0 && m_selectedHandleVertexIndex < vertices.size()) {
                    QPointF newTangent = vertices[m_selectedHandleVertexIndex].tangent;
                    // Get the tension for the side we were dragging
                    double newTension = (m_selectedHandleSide < 0)
                        ? vertices[m_selectedHandleVertexIndex].incomingTension
                        : vertices[m_selectedHandleVertexIndex].outgoingTension;

                    // Check if tangent/tension actually changed
                    QPointF tangentDelta = newTangent - m_handleStartTangent;
                    double tangentDist = std::sqrt(tangentDelta.x() * tangentDelta.x() + tangentDelta.y() * tangentDelta.y());
                    double tensionDiff = std::abs(newTension - m_handleStartTension);

                    if (tangentDist > 0.001 || tensionDiff > 0.001) {
                        ModifyHandleCommand* cmd = new ModifyHandleCommand(
                            m_selectedHandleObject,
                            m_selectedHandleVertexIndex,
                            m_selectedHandleSide,
                            m_handleStartTangent,
                            m_handleStartTension,
                            newTangent,
                            newTension
                        );
                        m_document->undoStack()->push(cmd);
                        m_document->notifyObjectChanged(m_selectedHandleObject);
                    }
                }
            }
        }
        showStatusMessage("Handle modified");
        m_mode = SelectMode::VertexSelected;  // Stay in vertex selected mode
        m_selectedHandleObject = nullptr;
        m_selectedHandleVertexIndex = -1;
        m_selectedHandleSide = 0;
    } else if (m_mode == SelectMode::DraggingVertex) {
        // Finish vertex drag - create undo command and notify document
        if (m_document && m_selectedVertexObject) {
            // Get final position with constraints applied
            QPointF finalPosition = m_currentPoint;

            // Apply multi-segment lock constraints (same as during dragging)
            if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
                finalPosition = constrainVertexWithLockedSegments(finalPosition, polyline, m_selectedVertexIndex);
            }

            // Only create undo command if position actually changed
            QPointF delta = finalPosition - m_vertexStartPosition;
            double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

            if (distance > 0.01) {
                MoveVertexCommand* cmd = new MoveVertexCommand(
                    m_selectedVertexObject,
                    m_selectedVertexIndex,
                    m_vertexStartPosition,
                    finalPosition
                );
                m_document->undoStack()->push(cmd);
            }

            m_document->notifyObjectChanged(m_selectedVertexObject);
        }
        showStatusMessage("Vertex moved");
        m_mode = SelectMode::None;
        m_selectedVertexObject = nullptr;
        m_selectedVertexIndex = -1;
        m_constrainedSegmentIndex = -1;
    } else if (m_mode == SelectMode::Selecting) {
        // Finish rectangular selection
        selectObjectsInRect(m_selectionRect, m_multiSelect);

        auto selected = m_document->selectedObjects();
        if (!selected.isEmpty()) {
            updateStatusMessage();
        } else {
            showStatusMessage("No objects in selection box");
        }

        m_mode = SelectMode::None;
        m_selectionRect = QRectF();
    } else if (m_mode != SelectMode::Dragging) {
        // Simple click (not a drag) - select object at point
        QPointF releasePoint = mapToScene(event->pos());
        QPointF delta = releasePoint - m_startPoint;
        double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (distance <= 5.0) {  // It's a click, not a drag
            selectObjectAt(releasePoint, m_multiSelect);
            updateStatusMessage();
        }
    }

    event->accept();
}

void SelectTool::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "SelectTool::keyPressEvent - key:" << event->key() << "text:" << event->text();

    if (event->key() == Qt::Key_Space) {
        if (m_mode == SelectMode::Dragging) {
            // Place the object(s)
            finishMove();
            m_mode = SelectMode::None;
            showStatusMessage("Object(s) placed");
        } else if (m_hoveredObject) {
            // Pick up the hovered object (has priority over already selected objects)
            if (!m_document) {
                return;
            }

            // Check if layer is locked
            if (m_document->isLayerLocked(m_hoveredObject->layer())) {
                showStatusMessage("Cannot move object on locked layer");
                event->accept();
                return;
            }

            // Clear previous selection and select this object
            m_document->clearSelection();
            QList<Geometry::GeometryObject*> selected;
            selected.append(m_hoveredObject);
            m_document->setSelectedObjects(selected);

            // Start dragging
            m_mode = SelectMode::Dragging;
            m_lastPoint = m_currentPoint;
            showStatusMessage(QString("Dragging %1 - Move and click/space to place")
                .arg(m_hoveredObject->typeName()));
        } else if (m_document && !m_document->selectedObjects().isEmpty()) {
            // Pick up all selected objects (only if no object is hovered)
            m_mode = SelectMode::Dragging;
            m_lastPoint = m_currentPoint;
            int count = m_document->selectedObjects().size();
            showStatusMessage(QString("Dragging %1 object(s) - Move and click/space to place").arg(count));
        }
        event->accept();
    } else if (event->key() == Qt::Key_Delete) {
        // Check if a vertex is selected/hovered first
        Geometry::GeometryObject* targetObject = nullptr;
        int targetIndex = -1;
        bool vertexHandled = false;

        if ((m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::VertexSelected) && m_selectedVertexObject) {
            targetObject = m_selectedVertexObject;
            targetIndex = m_selectedVertexIndex;
        } else if (m_hoveredVertexIndex >= 0 && m_document) {
            // Find the object containing the hovered vertex among selected objects
            auto selected = m_document->selectedObjects();
            for (auto* obj : selected) {
                if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
                    if (m_hoveredVertexIndex < polyline->vertices().size()) {
                        targetObject = obj;
                        targetIndex = m_hoveredVertexIndex;
                        break;
                    }
                }
            }
        }

        // If a vertex is selected/hovered, delete the vertex instead of the object
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(targetObject)) {
            // Check if layer is locked
            if (m_document && m_document->isLayerLocked(targetObject->layer())) {
                showStatusMessage("Cannot modify object on locked layer");
                vertexHandled = true;
            } else if (targetIndex >= 0 && targetIndex < polyline->vertexCount()) {
                if (polyline->vertexCount() <= 3) {
                    showStatusMessage("Cannot delete vertex - polyline needs at least 3 vertices");
                } else {
                    // Create undo command for deletion
                    auto* cmd = new DeleteVertexCommand(targetObject, targetIndex);
                    if (m_document && m_document->undoStack()) {
                        m_document->undoStack()->push(cmd);
                    } else {
                        cmd->redo();
                        delete cmd;
                    }
                    // Reset selection state
                    m_mode = SelectMode::None;
                    m_selectedVertexObject = nullptr;
                    m_selectedVertexIndex = -1;
                    m_hoveredVertexIndex = -1;
                    m_constrainedSegmentIndex = -1;
                    showStatusMessage(QString("Vertex deleted"));
                }
                vertexHandled = true;
            }
        }

        // Only delete objects if no vertex was handled
        if (!vertexHandled && targetObject == nullptr) {
            // No vertex selected - delete selected objects instead
            if (m_document) {
                auto selected = m_document->selectedObjects();
                if (!selected.isEmpty()) {
                    // Filter out objects on locked layers
                    QList<Geometry::GeometryObject*> deletableObjects;
                    int lockedCount = 0;
                    for (auto* obj : selected) {
                        if (m_document->isLayerLocked(obj->layer())) {
                            lockedCount++;
                        } else {
                            deletableObjects.append(obj);
                        }
                    }

                    if (deletableObjects.isEmpty()) {
                        showStatusMessage("Cannot delete objects on locked layers");
                        return;
                    }

                    // Confirmation for bulk delete
                    if (deletableObjects.size() > 10) {
                        QWidget* parentWidget = m_canvas ? m_canvas->parentWidget() : nullptr;
                        QMessageBox::StandardButton reply = QMessageBox::question(parentWidget,
                            QObject::tr("Delete Objects"),
                            QObject::tr("Delete %1 selected objects?").arg(deletableObjects.size()),
                            QMessageBox::Yes | QMessageBox::No);
                        if (reply != QMessageBox::Yes) {
                            return;
                        }
                    }

                    m_document->removeObjects(deletableObjects);
                    m_hoveredObject = nullptr;
                    if (lockedCount > 0) {
                        showStatusMessage(QString("Deleted %1 object(s) (skipped %2 on locked layers)")
                            .arg(deletableObjects.size()).arg(lockedCount));
                    } else {
                        showStatusMessage(QString("Deleted %1 object(s)").arg(deletableObjects.size()));
                    }
                }
            }
        }
        event->accept();
    } else if (event->key() == Qt::Key_Escape) {
        // Cancel drag or clear selection
        if (m_mode == SelectMode::DraggingHandle) {
            // Cancel handle drag - restore original values
            if (m_selectedHandleObject) {
                if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedHandleObject)) {
                    auto vertices = polyline->vertices();
                    if (m_selectedHandleVertexIndex >= 0 && m_selectedHandleVertexIndex < vertices.size()) {
                        vertices[m_selectedHandleVertexIndex].tangent = m_handleStartTangent;
                        if (m_selectedHandleSide < 0) {
                            vertices[m_selectedHandleVertexIndex].incomingTension = m_handleStartTension;
                        } else {
                            vertices[m_selectedHandleVertexIndex].outgoingTension = m_handleStartTension;
                        }
                        polyline->setVertices(vertices);
                        if (m_canvas) {
                            m_canvas->viewport()->update();
                        }
                    }
                }
            }
            m_mode = SelectMode::VertexSelected;
            m_selectedHandleObject = nullptr;
            m_selectedHandleVertexIndex = -1;
            m_selectedHandleSide = 0;
            showStatusMessage(QString("Handle drag cancelled"));
        } else if (m_mode == SelectMode::Dragging) {
            // TODO: Restore original position
            m_mode = SelectMode::None;
            showStatusMessage("Drag cancelled");
        } else if (m_mode == SelectMode::DraggingVertex) {
            // Cancel vertex drag - go back to selected
            m_mode = SelectMode::VertexSelected;
            showStatusMessage(QString("Vertex %1 selected | G: grab | L: lock | T: toggle type | Tab: dimension | Del: delete")
                .arg(m_selectedVertexIndex + 1));
        } else if (m_mode == SelectMode::VertexSelected) {
            // Deselect vertex
            m_mode = SelectMode::None;
            m_selectedVertexObject = nullptr;
            m_selectedVertexIndex = -1;
            m_constrainedSegmentIndex = -1;
            m_mode = SelectMode::None;
            updateStatusMessage();
        } else if (m_document) {
            m_document->clearSelection();
            updateStatusMessage();
        }
        event->accept();
    } else if (event->key() == Qt::Key_G) {
        qDebug() << "SelectTool: Key_G pressed, mode=" << (int)m_mode << "vertex selected=" << (m_selectedVertexObject != nullptr);
        // Activate move mode for selected vertex (G = Grab)
        if (m_mode == SelectMode::VertexSelected && m_selectedVertexObject) {
            // Check if layer is locked
            if (m_document && m_document->isLayerLocked(m_selectedVertexObject->layer())) {
                showStatusMessage("Cannot modify object on locked layer");
                event->accept();
                return;
            }

            m_mode = SelectMode::DraggingVertex;
            m_lastPoint = m_currentPoint;
            qDebug() << "SelectTool: Entering DraggingVertex mode";

            QString msg = QString("Moving vertex %1 | L: lock | T: toggle type | Tab: dimension | Esc: cancel")
                .arg(m_selectedVertexIndex + 1);
            if (m_constrainedSegmentIndex >= 0) {
                msg += " [LOCKED]";
            }
            showStatusMessage(msg);
        } else {
            qDebug() << "SelectTool: G pressed but conditions not met for vertex grab";
            showStatusMessage("Select a vertex first (click on a vertex to select it)");
        }
        event->accept();
    } else if (event->key() == Qt::Key_L) {
        // Check if we have selected segments to lock (multi-segment lock)
        if (!m_selectedSegments.isEmpty()) {
            lockSelectedSegments();
            event->accept();
            return;
        }

        // Otherwise: Legacy single-segment lock on vertex
        // Lock segment: works on dragged vertex, selected vertex, or hovered vertex
        // Allows locking BEFORE starting to move the vertex
        Geometry::GeometryObject* targetObject = nullptr;
        int targetIndex = -1;

        if ((m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::VertexSelected) && m_selectedVertexObject) {
            targetObject = m_selectedVertexObject;
            targetIndex = m_selectedVertexIndex;
        } else if (m_hoveredVertexIndex >= 0) {
            // Find the object containing the hovered vertex
            findVertexAt(m_currentPoint, &targetObject);
            targetIndex = m_hoveredVertexIndex;
        }

        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(targetObject)) {
            auto vertices = polyline->vertices();
            int n = vertices.size();
            int prevSegmentIdx = (targetIndex - 1 + n) % n;
            int nextSegmentIdx = targetIndex;

            // Get the vertex position (use m_currentPoint if dragging, otherwise use stored position)
            QPointF vertexPos = (m_mode == SelectMode::DraggingVertex) ? m_currentPoint : vertices[targetIndex].position;

            if (m_constrainedSegmentIndex == -1) {
                // First press: lock previous segment (use curve length, not straight distance)
                m_constrainedSegmentIndex = prevSegmentIdx;
                m_constrainedLength = polyline->calculateSegmentLength(prevSegmentIdx);
                showStatusMessage(QString("🔒 Previous segment locked to %1 (press L again for next)")
                    .arg(Units::formatLength(m_constrainedLength, 2)));
            } else if (m_constrainedSegmentIndex == prevSegmentIdx) {
                // Second press: switch to next segment
                m_constrainedSegmentIndex = nextSegmentIdx;
                m_constrainedLength = polyline->calculateSegmentLength(nextSegmentIdx);
                showStatusMessage(QString("🔒 Next segment locked to %1 (press L to unlock)")
                    .arg(Units::formatLength(m_constrainedLength, 2)));
            } else {
                // Third press: unlock
                m_constrainedSegmentIndex = -1;
                showStatusMessage("Segment unlocked (press L to lock previous)");
            }

            // If we're hovering but not dragging yet, store this for when drag starts
            if (m_mode != SelectMode::DraggingVertex) {
                m_selectedVertexObject = targetObject;
                m_selectedVertexIndex = targetIndex;
            }

            // Force viewport update to show locked segment immediately
            if (m_canvas) {
                m_canvas->viewport()->update();
            }
        }
        event->accept();
    } else if (event->key() == Qt::Key_Tab && m_mode == SelectMode::DraggingVertex) {
        // Request dimension input for free segment if one is locked
        if (m_constrainedSegmentIndex >= 0 && m_selectedVertexObject) {
            if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
                auto vertices = polyline->vertices();
                int n = vertices.size();
                int prevSegmentIdx = (m_selectedVertexIndex - 1 + n) % n;
                int nextSegmentIdx = m_selectedVertexIndex;

                // Determine which segment is free
                int freeSegmentIdx = -1;
                if (m_constrainedSegmentIndex == prevSegmentIdx) {
                    freeSegmentIdx = nextSegmentIdx;
                } else if (m_constrainedSegmentIndex == nextSegmentIdx) {
                    freeSegmentIdx = prevSegmentIdx;
                }

                if (freeSegmentIdx >= 0) {
                    // Calculate current length and angle of the free segment
                    double currentLength = polyline->calculateSegmentLength(freeSegmentIdx, m_selectedVertexIndex, m_currentPoint);

                    // Calculate angle of free segment
                    QPointF segmentStart, segmentEnd;
                    if (freeSegmentIdx == prevSegmentIdx) {
                        // Previous segment: from previous vertex to current
                        segmentStart = vertices[prevSegmentIdx].position;
                        segmentEnd = m_currentPoint;
                    } else {
                        // Next segment: from current to next vertex
                        segmentStart = m_currentPoint;
                        int endIdx = (freeSegmentIdx + 1) % n;
                        segmentEnd = vertices[endIdx].position;
                    }

                    QPointF delta = segmentEnd - segmentStart;
                    double angle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;

                    emit dimensionInputRequested("freeSegment", currentLength, angle);
                } else {
                    // No free segment (shouldn't happen)
                    showStatusMessage("No free segment to dimension");
                }
            }
        } else {
            // No segment is locked, can't apply dimension
            showStatusMessage("Lock a segment first (press L) to dimension the other one");
        }
        event->accept();
    } else if (event->key() == Qt::Key_Tab && m_selectedSegmentObject) {
        // Request dimension input for selected segment
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedSegmentObject)) {
            double currentLength = polyline->calculateSegmentLength(m_selectedSegmentIndex);

            // Calculate angle of segment
            auto vertices = polyline->vertices();
            int n = vertices.size();
            int endIdx = (m_selectedSegmentIndex + 1) % n;

            QPointF p1 = vertices[m_selectedSegmentIndex].position;
            QPointF p2 = vertices[endIdx].position;
            QPointF delta = p2 - p1;
            double angle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;

            emit dimensionInputRequested("segment", currentLength, angle);
        }
        event->accept();
    } else if (event->key() == Qt::Key_T) {
        // Toggle vertex type (Sharp <-> Smooth)
        // Works for selected, dragged, or hovered vertex
        Geometry::GeometryObject* targetObject = nullptr;
        int targetIndex = -1;

        if ((m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::VertexSelected) && m_selectedVertexObject) {
            targetObject = m_selectedVertexObject;
            targetIndex = m_selectedVertexIndex;
        } else if (m_hoveredVertexIndex >= 0) {
            // Find the object containing the hovered vertex
            findVertexAt(m_currentPoint, &targetObject);
            targetIndex = m_hoveredVertexIndex;
        }

        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(targetObject)) {
            // Check if layer is locked
            if (m_document && m_document->isLayerLocked(targetObject->layer())) {
                showStatusMessage("Cannot modify object on locked layer");
                event->accept();
                return;
            }

            if (targetIndex >= 0 && targetIndex < polyline->vertexCount()) {
                auto vertex = polyline->vertexAt(targetIndex);
                Geometry::VertexType oldType = vertex.type;
                Geometry::VertexType newType = (oldType == Geometry::VertexType::Sharp)
                    ? Geometry::VertexType::Smooth
                    : Geometry::VertexType::Sharp;

                // Create undo command for type change
                auto* cmd = new ChangeVertexTypeCommand(
                    targetObject,
                    targetIndex,
                    static_cast<int>(oldType),
                    static_cast<int>(newType)
                );

                if (m_document && m_document->undoStack()) {
                    m_document->undoStack()->push(cmd);
                } else {
                    cmd->redo();
                    delete cmd;
                }

                QString typeName = (newType == Geometry::VertexType::Sharp) ? "Sharp" : "Smooth";
                showStatusMessage(QString("Vertex %1 changed to %2").arg(targetIndex + 1).arg(typeName));
            }
        }
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void SelectTool::drawOverlay(QPainter* painter)
{
    painter->save();

    // Draw selection rectangle if in Selecting mode
    if (m_mode == SelectMode::Selecting) {
        QPen selectionPen(QColor(0, 120, 215), 2, Qt::DashLine); // Blue
        QBrush selectionBrush(QColor(0, 120, 215, 30));  // Semi-transparent blue
        painter->setPen(selectionPen);
        painter->setBrush(selectionBrush);
        painter->drawRect(m_selectionRect);
    }

    // Note: Hover and selection highlights are now drawn directly on the polyline
    // in Polyline::draw() instead of drawing bounding boxes

    // Draw vertex handles for selected objects (only if layer is visible)
    if (m_document) {
        auto selected = m_document->selectedObjects();
        for (auto* obj : selected) {
            // Skip if object's layer is not visible
            if (!m_document->isLayerVisible(obj->layer())) {
                continue;
            }

            // Draw vertex handles for polylines
            if (m_mode != SelectMode::Dragging && m_mode != SelectMode::Selecting) {
                drawVertexHandles(painter, obj);
            }
        }
    }

    // Draw curve handles for selected smooth vertex (only if layer is visible)
    if ((m_mode == SelectMode::VertexSelected || m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::DraggingHandle)
        && m_selectedVertexObject) {
        // Skip if object's layer is not visible
        if (m_document && !m_document->isLayerVisible(m_selectedVertexObject->layer())) {
            painter->restore();
            return;
        }

        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            auto vertices = polyline->vertices();
            if (m_selectedVertexIndex >= 0 && m_selectedVertexIndex < vertices.size()) {
                if (vertices[m_selectedVertexIndex].type == Geometry::VertexType::Smooth) {
                    drawCurveHandles(painter, m_selectedVertexObject, m_selectedVertexIndex);

                    // Display segment length while dragging handle
                    if (m_mode == SelectMode::DraggingHandle) {
                        int n = vertices.size();
                        // Determine which segment is affected by this handle
                        int affectedSegment = (m_selectedHandleSide < 0)
                            ? (m_selectedVertexIndex - 1 + n) % n  // Incoming handle affects previous segment
                            : m_selectedVertexIndex;                // Outgoing handle affects this segment

                        // Calculate the length of the affected segment
                        double segmentLength = polyline->calculateSegmentLength(affectedSegment);

                        QString lengthText = Units::formatLength(segmentLength, 2);
                        if (m_constrainedSegmentIndex == affectedSegment) {
                            lengthText = "🔒 " + lengthText;
                        }

                        QFont dimFont;
                        dimFont.setPointSize(11);
                        dimFont.setBold(true);
                        painter->setFont(dimFont);

                        // Position the text near the middle of the affected segment
                        QPointF p1 = vertices[affectedSegment].position;
                        int nextIdx = (affectedSegment + 1) % n;
                        QPointF p2 = vertices[nextIdx].position;
                        QPointF midpoint = (p1 + p2) / 2.0;

                        QFontMetrics fm(dimFont);
                        QRect textRect = fm.boundingRect(lengthText);
                        textRect.moveCenter(midpoint.toPoint());
                        textRect.adjust(-5, -3, 5, 3);

                        painter->setPen(Qt::NoPen);
                        painter->setBrush(QColor(100, 150, 255, 240));
                        painter->drawRect(textRect);

                        painter->setPen(Qt::white);
                        painter->drawText(textRect, Qt::AlignCenter, lengthText);
                    } else if (m_mode == SelectMode::VertexSelected) {
                        // Show lengths of both adjacent segments when vertex is selected but not dragging
                        int n = vertices.size();
                        int prevSegmentIdx = (m_selectedVertexIndex - 1 + n) % n;
                        int nextSegmentIdx = m_selectedVertexIndex;

                        QFont dimFont;
                        dimFont.setPointSize(10);
                        dimFont.setBold(false);
                        painter->setFont(dimFont);

                        // Incoming segment (previous)
                        double prevLength = polyline->calculateSegmentLength(prevSegmentIdx);
                        QString prevText = Units::formatLength(prevLength, 2);
                        if (m_constrainedSegmentIndex == prevSegmentIdx) {
                            prevText = "🔒 " + prevText;
                        }

                        QPointF p0 = vertices[prevSegmentIdx].position;
                        QPointF p1 = vertices[m_selectedVertexIndex].position;
                        QPointF prevMidpoint = (p0 + p1) / 2.0;

                        QFontMetrics fm(dimFont);
                        QRect prevRect = fm.boundingRect(prevText);
                        prevRect.moveCenter((prevMidpoint + QPointF(0, -15)).toPoint());
                        prevRect.adjust(-3, -2, 3, 2);

                        painter->setPen(Qt::NoPen);
                        painter->setBrush(QColor(200, 200, 200, 200));
                        painter->drawRect(prevRect);
                        painter->setPen(QColor(60, 60, 60));
                        painter->drawText(prevRect, Qt::AlignCenter, prevText);

                        // Outgoing segment (next)
                        double nextLength = polyline->calculateSegmentLength(nextSegmentIdx);
                        QString nextText = Units::formatLength(nextLength, 2);
                        if (m_constrainedSegmentIndex == nextSegmentIdx) {
                            nextText = "🔒 " + nextText;
                        }

                        int nextIdx = (m_selectedVertexIndex + 1) % n;
                        QPointF p2 = vertices[nextIdx].position;
                        QPointF nextMidpoint = (p1 + p2) / 2.0;

                        QRect nextRect = fm.boundingRect(nextText);
                        nextRect.moveCenter((nextMidpoint + QPointF(0, -15)).toPoint());
                        nextRect.adjust(-3, -2, 3, 2);

                        painter->setPen(Qt::NoPen);
                        painter->setBrush(QColor(200, 200, 200, 200));
                        painter->drawRect(nextRect);
                        painter->setPen(QColor(60, 60, 60));
                        painter->drawText(nextRect, Qt::AlignCenter, nextText);
                    }
                }
            }
        }
    }

    // Draw selected segment indicator (only if layer is visible)
    if (m_selectedSegmentObject && m_selectedSegmentIndex >= 0) {
        // Skip if object's layer is not visible
        if (m_document && !m_document->isLayerVisible(m_selectedSegmentObject->layer())) {
            painter->restore();
            return;
        }

        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedSegmentObject)) {
            auto vertices = polyline->vertices();
            int n = vertices.size();

            if (m_selectedSegmentIndex < n) {
                const Geometry::PolylineVertex& current = vertices[m_selectedSegmentIndex];
                int endIdx = (m_selectedSegmentIndex + 1) % n;
                const Geometry::PolylineVertex& next = vertices[endIdx];

                painter->save();
                QPen selectPen(QColor(0, 200, 0), 3, Qt::SolidLine);
                painter->setBrush(Qt::NoBrush);

                // Check if curved
                bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                                  (next.type == Geometry::VertexType::Smooth);

                QPainterPath path;
                path.moveTo(current.position);

                if (needsCurve) {
                    QPointF p1 = current.position;
                    QPointF p2 = next.position;
                    QPointF segment = p2 - p1;
                    double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
                    double controlDistance = dist / 3.0;

                    QPointF c1, c2;

                    if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
                        c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
                    } else if (current.type == Geometry::VertexType::Smooth) {
                        int prevIdx = (m_selectedSegmentIndex - 1 + n) % n;
                        QPointF p0 = vertices[prevIdx].position;
                        c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
                    } else {
                        c1 = p1 + (p2 - p1) * 0.01;
                    }

                    if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
                        c2 = p2 - next.tangent * controlDistance * next.incomingTension;
                    } else if (next.type == Geometry::VertexType::Smooth) {
                        int nextNextIdx = (m_selectedSegmentIndex + 2) % n;
                        QPointF p3 = vertices[nextNextIdx].position;
                        c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
                    } else {
                        c2 = p2 - (p2 - p1) * 0.01;
                    }

                    path.cubicTo(c1, c2, p2);
                } else {
                    path.lineTo(next.position);
                }

                painter->strokePath(path, selectPen);
                painter->restore();
            }
        }
    }

    // Draw locked segment indicator (even when not dragging)
    if (m_constrainedSegmentIndex >= 0 && m_selectedVertexObject) {
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            auto vertices = polyline->vertices();
            int n = vertices.size();

            if (m_constrainedSegmentIndex < n) {
                const Geometry::PolylineVertex& current = vertices[m_constrainedSegmentIndex];
                int endIdx = (m_constrainedSegmentIndex + 1) % n;
                const Geometry::PolylineVertex& next = vertices[endIdx];

                QPointF p1 = current.position;
                QPointF p2 = next.position;

                painter->save();
                QPen lockPen(QColor(255, 0, 0), 4, Qt::SolidLine);
                painter->setPen(lockPen);
                painter->setBrush(Qt::NoBrush);

                // Check if this segment is curved
                bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                                  (next.type == Geometry::VertexType::Smooth);

                QPainterPath lockPath;
                lockPath.moveTo(p1);

                if (needsCurve) {
                    // Calculate control points for cubic Bezier (same logic as in Polyline::createPath)
                    QPointF segment = p2 - p1;
                    double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
                    double controlDistance = dist / 3.0;

                    QPointF c1, c2;

                    // Control point c1 (outgoing from current)
                    if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
                        c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
                    } else if (current.type == Geometry::VertexType::Smooth) {
                        int prevIdx = (m_constrainedSegmentIndex - 1 + n) % n;
                        QPointF p0 = vertices[prevIdx].position;
                        if (!polyline->isClosed() && m_constrainedSegmentIndex == 0) p0 = p1;
                        c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
                    } else {
                        c1 = p1 + (p2 - p1) * 0.01;
                    }

                    // Control point c2 (incoming to next)
                    if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
                        c2 = p2 - next.tangent * controlDistance * next.incomingTension;
                    } else if (next.type == Geometry::VertexType::Smooth) {
                        int nextNextIdx = (m_constrainedSegmentIndex + 2) % n;
                        QPointF p3 = vertices[nextNextIdx].position;
                        if (!polyline->isClosed() && endIdx == n - 1) p3 = p2;
                        c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
                    } else {
                        c2 = p2 - (p2 - p1) * 0.01;
                    }

                    // Draw the Bezier curve
                    lockPath.cubicTo(c1, c2, p2);
                } else {
                    // Straight line
                    lockPath.lineTo(p2);
                }

                painter->strokePath(lockPath, lockPen);

                // Draw lock icon/text at midpoint (for curve, calculate t=0.5 point)
                QPointF midpoint;
                if (needsCurve) {
                    // Calculate point at t=0.5 on the Bezier curve for better positioning
                    midpoint = (p1 + p2) / 2.0; // Simple approximation
                } else {
                    midpoint = (p1 + p2) / 2.0;
                }

                QString lockText = QString("🔒 %1").arg(Units::formatLength(m_constrainedLength, 2));

                QFont lockFont;
                lockFont.setPointSize(11);
                lockFont.setBold(true);
                painter->setFont(lockFont);

                QFontMetrics fm(lockFont);
                QRect textRect = fm.boundingRect(lockText);
                textRect.moveCenter(midpoint.toPoint());
                textRect.adjust(-5, -3, 5, 3);

                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(255, 100, 100, 240));
                painter->drawRect(textRect);

                painter->setPen(Qt::white);
                painter->drawText(textRect, Qt::AlignCenter, lockText);

                painter->restore();
            }
        }
    }

    // Draw dimensions when dragging a vertex
    if (m_mode == SelectMode::DraggingVertex && m_selectedVertexObject) {
        QFont dimFont;
        dimFont.setPointSize(10);
        dimFont.setBold(true);
        painter->setFont(dimFont);

        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            auto vertices = polyline->vertices();
            int n = vertices.size();

            // Show dimensions of adjacent segments
            if (m_selectedVertexIndex >= 0 && m_selectedVertexIndex < n) {
                // Previous segment
                int prevIdx = (m_selectedVertexIndex - 1 + n) % n;
                QPointF p0 = vertices[prevIdx].position;
                QPointF p1 = m_currentPoint; // Current dragged position

                QPointF delta0 = p1 - p0;
                double length0 = std::sqrt(delta0.x() * delta0.x() + delta0.y() * delta0.y());

                if (length0 > 1.0) {
                    int prevSegmentIdx = (m_selectedVertexIndex - 1 + n) % n;
                    bool isConstrained = (m_constrainedSegmentIndex == prevSegmentIdx);
                    QString lengthText = Units::formatLength(length0, 2);
                    if (isConstrained) {
                        lengthText = "🔒 " + lengthText; // Lock icon
                    }

                    QPointF midpoint = (p0 + p1) / 2.0;
                    QPointF perpendicular(-delta0.y(), delta0.x());
                    double perpLength = std::sqrt(perpendicular.x() * perpendicular.x() +
                                                perpendicular.y() * perpendicular.y());
                    if (perpLength > 0.001) {
                        perpendicular /= perpLength;
                        perpendicular *= 15.0;
                    }
                    QPointF textPos = midpoint + perpendicular;

                    QFontMetrics fm(dimFont);
                    QRect textRect = fm.boundingRect(lengthText);
                    textRect.moveCenter(textPos.toPoint());

                    painter->setPen(Qt::NoPen);
                    if (isConstrained) {
                        painter->setBrush(QColor(255, 100, 100, 240)); // Red for locked
                        // Note: thicker line removed - already drawn by legacy lock system
                    } else {
                        painter->setBrush(QColor(255, 255, 100, 240)); // Yellow
                    }
                    painter->drawRect(textRect.adjusted(-3, -2, 3, 2));

                    painter->setPen(isConstrained ? Qt::red : QColor(180, 0, 0));
                    painter->drawText(textRect, Qt::AlignCenter, lengthText);
                }

                // Next segment
                int nextIdx = (m_selectedVertexIndex + 1) % n;
                QPointF p2 = vertices[nextIdx].position;

                QPointF delta1 = p2 - p1;
                double length1 = std::sqrt(delta1.x() * delta1.x() + delta1.y() * delta1.y());

                if (length1 > 1.0) {
                    int nextSegmentIdx = m_selectedVertexIndex;
                    bool isConstrained = (m_constrainedSegmentIndex == nextSegmentIdx);
                    QString lengthText = Units::formatLength(length1, 2);
                    if (isConstrained) {
                        lengthText = "🔒 " + lengthText; // Lock icon
                    }

                    QPointF midpoint = (p1 + p2) / 2.0;
                    QPointF perpendicular(-delta1.y(), delta1.x());
                    double perpLength = std::sqrt(perpendicular.x() * perpendicular.x() +
                                                perpendicular.y() * perpendicular.y());
                    if (perpLength > 0.001) {
                        perpendicular /= perpLength;
                        perpendicular *= 15.0;
                    }
                    QPointF textPos = midpoint + perpendicular;

                    QFontMetrics fm(dimFont);
                    QRect textRect = fm.boundingRect(lengthText);
                    textRect.moveCenter(textPos.toPoint());

                    painter->setPen(Qt::NoPen);
                    if (isConstrained) {
                        painter->setBrush(QColor(255, 100, 100, 240)); // Red for locked
                        // Note: thicker line removed - already drawn by legacy lock system
                    } else {
                        painter->setBrush(QColor(255, 255, 100, 240)); // Yellow
                    }
                    painter->drawRect(textRect.adjusted(-3, -2, 3, 2));

                    painter->setPen(isConstrained ? Qt::red : QColor(180, 0, 0));
                    painter->drawText(textRect, Qt::AlignCenter, lengthText);
                }
            }
        } else if (auto* line = dynamic_cast<Geometry::Line*>(m_selectedVertexObject)) {
            // For lines, show the line length
            QPointF p1, p2;
            if (m_selectedVertexIndex == 0) {
                p1 = m_currentPoint;
                p2 = line->end();
            } else {
                p1 = line->start();
                p2 = m_currentPoint;
            }

            QPointF delta = p2 - p1;
            double length = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

            if (length > 1.0) {
                QString lengthText = Units::formatLength(length, 2);
                QPointF midpoint = (p1 + p2) / 2.0;
                QPointF perpendicular(-delta.y(), delta.x());
                double perpLength = std::sqrt(perpendicular.x() * perpendicular.x() +
                                            perpendicular.y() * perpendicular.y());
                if (perpLength > 0.001) {
                    perpendicular /= perpLength;
                    perpendicular *= 15.0;
                }
                QPointF textPos = midpoint + perpendicular;

                QFontMetrics fm(dimFont);
                QRect textRect = fm.boundingRect(lengthText);
                textRect.moveCenter(textPos.toPoint());

                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(255, 255, 100, 240)); // Bright yellow
                painter->drawRect(textRect.adjusted(-3, -2, 3, 2));

                painter->setPen(QColor(180, 0, 0)); // Dark red
                painter->drawText(textRect, Qt::AlignCenter, lengthText);
            }
        }
    }

    // Draw selected segments (for multi-lock) - orange dashed
    drawSelectedSegments(painter);

    // Draw locked segments (for multi-lock) - red thick with lock icon
    drawLockedSegments(painter);

    painter->restore();
}

void SelectTool::selectObjectAt(const QPointF& point, bool addToSelection)
{
    if (!m_document) {
        return;
    }

    Geometry::GeometryObject* obj = findObjectAt(point);

    // Check if layer is locked
    if (obj && m_document->isLayerLocked(obj->layer())) {
        showStatusMessage("Cannot select object on locked layer");
        return;
    }

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
        int lockedCount = 0;
        for (auto* obj : objects) {
            // Skip objects on locked layers
            if (m_document->isLayerLocked(obj->layer())) {
                lockedCount++;
                continue;
            }
            if (!selected.contains(obj)) {
                selected.append(obj);
            }
        }
        m_document->setSelectedObjects(selected);
        if (lockedCount > 0) {
            showStatusMessage(QString("Skipped %1 object(s) on locked layers").arg(lockedCount));
        }
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

    // Apply delta to selected objects
    for (auto* obj : m_document->selectedObjects()) {
        obj->translate(delta);
    }
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

void SelectTool::showContextMenu(const QPoint& globalPos)
{
    if (!m_document) {
        return;
    }

    auto selected = m_document->selectedObjects();
    if (selected.isEmpty()) {
        return;
    }

    QMenu menu;

    // Transformation tools
    QAction* rotateAction = menu.addAction(tr("🔄 Rotate (R)"));
    connect(rotateAction, &QAction::triggered, [this]() {
        // Signal to switch to Rotate tool
        emit toolChangeRequested("Rotate");
    });

    QAction* mirrorAction = menu.addAction(tr("↔️ Mirror (M)"));
    connect(mirrorAction, &QAction::triggered, [this]() {
        // Signal to switch to Mirror tool
        emit toolChangeRequested("Mirror");
    });

    QAction* scaleAction = menu.addAction(tr("📐 Scale (S)"));
    connect(scaleAction, &QAction::triggered, [this]() {
        // Signal to switch to Scale tool
        emit toolChangeRequested("Scale");
    });

    menu.addSeparator();

    // Change Layer action
    QAction* changeLayerAction = menu.addAction("Change Layer...");
    connect(changeLayerAction, &QAction::triggered, [this]() {
        if (!m_document) {
            return;
        }

        auto selected = m_document->selectedObjects();
        if (selected.isEmpty()) {
            return;
        }

        // Get list of available layers
        QStringList layers = m_document->layers();
        if (layers.isEmpty()) {
            return;
        }

        // Get current layer of first selected object
        QString currentLayer = selected.first()->layer();

        // Show dialog to select new layer
        bool ok;
        QString newLayer = QInputDialog::getItem(
            nullptr,
            "Change Layer",
            "Select target layer:",
            layers,
            layers.indexOf(currentLayer),
            false,
            &ok
        );

        if (ok && !newLayer.isEmpty()) {
            // Use command for undo/redo support
            ChangeLayersCommand* cmd = new ChangeLayersCommand(selected, newLayer);
            m_document->undoStack()->push(cmd);

            showStatusMessage(QString("Moved %1 object(s) to layer '%2'")
                .arg(selected.size())
                .arg(newLayer));
        }
    });

    menu.addSeparator();

    // Delete action
    QAction* deleteAction = menu.addAction(tr("🗑️ Delete (Del)"));
    connect(deleteAction, &QAction::triggered, [this]() {
        if (!m_document) {
            return;
        }

        auto selected = m_document->selectedObjects();
        if (!selected.isEmpty()) {
            m_document->removeObjects(selected);
            m_hoveredObject = nullptr;
            showStatusMessage(QString("Deleted %1 object(s)").arg(selected.size()));
        }
    });

    menu.exec(globalPos);
}

int SelectTool::findVertexAt(const QPointF& point, Geometry::GeometryObject** outObject) const
{
    if (!m_document) {
        return -1;
    }

    const double tolerance = 8.0;

    // Check selected objects first for better UX
    auto selected = m_document->selectedObjects();
    for (auto* obj : selected) {
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
            int vertexIdx = polyline->findVertexAt(point, tolerance);
            if (vertexIdx >= 0) {
                if (outObject) *outObject = obj;
                return vertexIdx;
            }
        } else if (auto* line = dynamic_cast<Geometry::Line*>(obj)) {
            // Check start and end points
            QPointF delta1 = line->start() - point;
            double dist1 = std::sqrt(delta1.x() * delta1.x() + delta1.y() * delta1.y());
            if (dist1 <= tolerance) {
                if (outObject) *outObject = obj;
                return 0; // Start point
            }

            QPointF delta2 = line->end() - point;
            double dist2 = std::sqrt(delta2.x() * delta2.x() + delta2.y() * delta2.y());
            if (dist2 <= tolerance) {
                if (outObject) *outObject = obj;
                return 1; // End point
            }
        }
    }

    return -1;
}

void SelectTool::drawVertexHandles(QPainter* painter, Geometry::GeometryObject* obj) const
{
    if (!obj) return;

    const double handleSize = 6.0;
    const double hoveredHandleSize = 8.0;

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
        auto vertices = polyline->vertices();
        for (int i = 0; i < vertices.size(); ++i) {
            QPointF pos = vertices[i].position;
            bool isHovered = (m_hoveredVertexIndex == i);
            bool isSelected = ((m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::VertexSelected) &&
                             m_selectedVertexObject == obj &&
                             m_selectedVertexIndex == i);

            // Choose color based on vertex type
            QColor color;
            if (isSelected) {
                color = QColor(0, 255, 0); // Green for selected/dragging
            } else if (isHovered) {
                color = QColor(255, 165, 0); // Orange for hovered
            } else if (vertices[i].type == Geometry::VertexType::Smooth) {
                color = QColor(100, 149, 237); // Cornflower blue for smooth
            } else {
                color = Qt::blue; // Blue for sharp
            }

            double size = isHovered ? hoveredHandleSize : handleSize;

            painter->setPen(QPen(color, 2));
            painter->setBrush(QBrush(Qt::white));

            // Draw different shapes for different vertex types
            if (vertices[i].type == Geometry::VertexType::Smooth) {
                // Circle for smooth vertices
                painter->drawEllipse(pos, size, size);
            } else {
                // Square for sharp vertices
                painter->drawRect(QRectF(
                    pos.x() - size,
                    pos.y() - size,
                    size * 2,
                    size * 2
                ));
            }
        }
    } else if (auto* line = dynamic_cast<Geometry::Line*>(obj)) {
        // Draw handles for line start and end points
        QPointF points[2] = { line->start(), line->end() };

        for (int i = 0; i < 2; ++i) {
            bool isHovered = (m_hoveredVertexIndex == i);
            bool isSelected = ((m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::VertexSelected) &&
                             m_selectedVertexObject == obj &&
                             m_selectedVertexIndex == i);

            QColor color;
            if (isSelected) {
                color = QColor(0, 255, 0); // Green
            } else if (isHovered) {
                color = QColor(255, 165, 0); // Orange
            } else {
                color = Qt::blue;
            }

            double size = isHovered ? hoveredHandleSize : handleSize;

            painter->setPen(QPen(color, 2));
            painter->setBrush(QBrush(Qt::white));
            painter->drawRect(QRectF(
                points[i].x() - size,
                points[i].y() - size,
                size * 2,
                size * 2
            ));
        }
    }
}

void SelectTool::applyFreeSegmentLength(double lengthInMm, double angleDegrees)
{
    if ((m_mode != SelectMode::DraggingVertex && m_mode != SelectMode::VertexSelected) || !m_selectedVertexObject) {
        return;
    }

    if (m_constrainedSegmentIndex < 0) {
        showStatusMessage("No segment is locked");
        return;
    }

    auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject);
    if (!polyline) {
        return;
    }

    auto vertices = polyline->vertices();
    int n = vertices.size();
    int prevSegmentIdx = (m_selectedVertexIndex - 1 + n) % n;
    int nextSegmentIdx = m_selectedVertexIndex;

    // Determine which segment is free
    int freeSegmentIdx = -1;
    QPointF fixedPoint;

    if (m_constrainedSegmentIndex == prevSegmentIdx) {
        // Previous segment is locked, next is free
        freeSegmentIdx = nextSegmentIdx;
        fixedPoint = vertices[prevSegmentIdx].position;
    } else if (m_constrainedSegmentIndex == nextSegmentIdx) {
        // Next segment is locked, previous is free
        freeSegmentIdx = prevSegmentIdx;
        int endIdx = (nextSegmentIdx + 1) % n;
        fixedPoint = vertices[endIdx].position;
    } else {
        showStatusMessage("Error: constrained segment not adjacent to vertex");
        return;
    }

    // Calculate new position based on the desired length and angle
    // For the free segment, we need to position the vertex such that:
    // 1. The constrained segment maintains its length
    // 2. The free segment has the specified length and angle

    // Start from the fixed point (the other end of the free segment)
    QPointF freeSegmentFixedEnd;
    if (freeSegmentIdx == prevSegmentIdx) {
        // Free is previous: fixed end is the previous vertex
        freeSegmentFixedEnd = vertices[prevSegmentIdx].position;
    } else {
        // Free is next: fixed end is the next vertex
        int endIdx = (freeSegmentIdx + 1) % n;
        freeSegmentFixedEnd = vertices[endIdx].position;
    }

    // Calculate desired position for vertex using angle and length
    double angleRad = angleDegrees * M_PI / 180.0;
    QPointF direction(std::cos(angleRad), std::sin(angleRad));

    // For straight segments, it's simple
    QPointF desiredPosition;
    if (freeSegmentIdx == prevSegmentIdx) {
        // Previous segment: move from previous vertex by length in direction
        desiredPosition = freeSegmentFixedEnd + direction * lengthInMm;
    } else {
        // Next segment: the direction goes FROM current TO next
        // So we need to reverse: current = next - direction * length
        int endIdx = (freeSegmentIdx + 1) % n;
        freeSegmentFixedEnd = vertices[endIdx].position;
        desiredPosition = freeSegmentFixedEnd - direction * lengthInMm;
    }

    // Now adjust iteratively to maintain the constrained segment length
    // (similar to the constraint logic in mouseMoveEvent)
    QPointF testPosition = desiredPosition;
    const int maxIterations = 10;

    for (int iter = 0; iter < maxIterations; ++iter) {
        double currentLength = polyline->calculateSegmentLength(
            m_constrainedSegmentIndex,
            m_selectedVertexIndex,
            testPosition
        );

        double error = currentLength - m_constrainedLength;
        if (std::abs(error) < 0.1) {
            break;
        }

        // Adjust position to maintain constrained length
        QPointF constraintFixedPoint;
        if (m_constrainedSegmentIndex == prevSegmentIdx) {
            constraintFixedPoint = vertices[prevSegmentIdx].position;
        } else {
            int endIdx = (m_constrainedSegmentIndex + 1) % n;
            constraintFixedPoint = vertices[endIdx].position;
        }

        QPointF dir = testPosition - constraintFixedPoint;
        double dist = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());

        if (dist > 0.001) {
            dir /= dist;
            double adjustment = -error * 0.7;
            testPosition = constraintFixedPoint + dir * (dist + adjustment);
        } else {
            break;
        }
    }

    // Update the current point and apply
    m_currentPoint = testPosition;

    // Create undo command if position changed significantly
    QPointF delta = testPosition - m_vertexStartPosition;
    double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

    if (distance > 0.01) {
        polyline->updateVertex(m_selectedVertexIndex, m_currentPoint);

        // Create undo command
        if (m_document) {
            MoveVertexCommand* cmd = new MoveVertexCommand(
                m_selectedVertexObject,
                m_selectedVertexIndex,
                m_vertexStartPosition,
                testPosition
            );
            m_document->undoStack()->push(cmd);
            m_document->notifyObjectChanged(m_selectedVertexObject);
        }

        // Update start position for next operation
        m_vertexStartPosition = testPosition;
    }

    if (m_canvas) {
        m_canvas->viewport()->update();
    }

    showStatusMessage(QString("Free segment set to %1 at %2°")
        .arg(Units::formatLength(lengthInMm, 2))
        .arg(angleDegrees, 0, 'f', 1));
}

int SelectTool::findSegmentAt(const QPointF& point, Geometry::GeometryObject** outObject) const
{
    if (!m_document) {
        return -1;
    }

    const double tolerance = 5.0; // pixels

    // Check all objects in document
    for (auto* obj : m_document->objects()) {
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
            int segmentIndex = polyline->findClosestSegment(point);
            if (segmentIndex >= 0) {
                // Check if we're actually close enough to the segment
                QPointF closestPoint;
                polyline->findClosestSegment(point, &closestPoint);
                QPointF delta = point - closestPoint;
                double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (dist <= tolerance) {
                    if (outObject) {
                        *outObject = obj;
                    }
                    return segmentIndex;
                }
            }
        } else if (auto* line = dynamic_cast<Geometry::Line*>(obj)) {
            // For a line, there's only one segment (index 0)
            QPointF p1 = line->start();
            QPointF p2 = line->end();

            // Project point onto line segment
            QPointF segment = p2 - p1;
            QPointF toPoint = point - p1;
            double segmentLength = segment.x() * segment.x() + segment.y() * segment.y();

            if (segmentLength < 0.0001) continue;

            double t = (toPoint.x() * segment.x() + toPoint.y() * segment.y()) / segmentLength;
            t = qBound(0.0, t, 1.0);

            QPointF projected = p1 + segment * t;
            QPointF delta = point - projected;
            double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

            if (distance <= tolerance) {
                if (outObject) {
                    *outObject = obj;
                }
                return 0; // Line only has one segment
            }
        }
    }

    return -1;
}

void SelectTool::applySegmentLength(double lengthInMm, double angleDegrees, UI::ResizeMode mode)
{
    if (!m_selectedSegmentObject) {
        showStatusMessage("No segment selected");
        return;
    }

    auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedSegmentObject);
    if (!polyline) {
        showStatusMessage("Segment dimension only works for polylines");
        return;
    }

    auto vertices = polyline->vertices();
    int n = vertices.size();

    if (m_selectedSegmentIndex < 0 || m_selectedSegmentIndex >= n) {
        return;
    }

    // Get the segment's two vertices
    int startIdx = m_selectedSegmentIndex;
    int endIdx = (m_selectedSegmentIndex + 1) % n;

    QPointF startPos = vertices[startIdx].position;
    QPointF endPos = vertices[endIdx].position;

    // Calculate direction
    double angleRad = angleDegrees * M_PI / 180.0;
    QPointF direction(std::cos(angleRad), std::sin(angleRad));

    // Determine new positions based on resize mode
    QPointF newStartPos = startPos;
    QPointF newEndPos = endPos;

    switch (mode) {
        case UI::ResizeMode::FixStart:
            // Start point fixed, end point moves
            newEndPos = startPos + direction * lengthInMm;
            break;

        case UI::ResizeMode::FixEnd:
            // End point fixed, start point moves
            newStartPos = endPos - direction * lengthInMm;
            break;

        case UI::ResizeMode::Center: {
            // Both points move symmetrically from center
            QPointF midpoint = (startPos + endPos) / 2.0;
            QPointF halfOffset = direction * (lengthInMm / 2.0);
            newStartPos = midpoint - halfOffset;
            newEndPos = midpoint + halfOffset;
            break;
        }
    }

    // Update vertices and create undo commands
    if (m_document) {
        m_document->undoStack()->beginMacro("Resize Segment");

        // Update start vertex if it changed
        if ((newStartPos - startPos).manhattanLength() > 0.01) {
            polyline->updateVertex(startIdx, newStartPos);
            MoveVertexCommand* cmd = new MoveVertexCommand(
                m_selectedSegmentObject,
                startIdx,
                startPos,
                newStartPos
            );
            m_document->undoStack()->push(cmd);
        }

        // Update end vertex if it changed
        if ((newEndPos - endPos).manhattanLength() > 0.01) {
            polyline->updateVertex(endIdx, newEndPos);
            MoveVertexCommand* cmd = new MoveVertexCommand(
                m_selectedSegmentObject,
                endIdx,
                endPos,
                newEndPos
            );
            m_document->undoStack()->push(cmd);
        }

        m_document->undoStack()->endMacro();
        m_document->notifyObjectChanged(m_selectedSegmentObject);
    }

    if (m_canvas) {
        m_canvas->viewport()->update();
    }

    QString modeStr;
    switch (mode) {
        case UI::ResizeMode::FixStart: modeStr = "start fixed"; break;
        case UI::ResizeMode::FixEnd: modeStr = "end fixed"; break;
        case UI::ResizeMode::Center: modeStr = "centered"; break;
    }

    showStatusMessage(QString("Segment resized to %1 at %2° (%3)")
        .arg(Units::formatLength(lengthInMm, 2))
        .arg(angleDegrees, 0, 'f', 1)
        .arg(modeStr));
}

QPointF SelectTool::getHandlePosition(Geometry::Polyline* polyline, int vertexIndex, int side) const
{
    auto vertices = polyline->vertices();
    int n = vertices.size();

    if (vertexIndex < 0 || vertexIndex >= n) {
        return QPointF();
    }

    const Geometry::PolylineVertex& vertex = vertices[vertexIndex];

    // Only smooth vertices have handles
    if (vertex.type != Geometry::VertexType::Smooth) {
        return QPointF();
    }

    QPointF vertexPos = vertex.position;

    // If explicit tangent is set, use it
    if (vertex.tangent != QPointF()) {
        QPointF tangent = vertex.tangent;
        // Normalize
        double len = std::sqrt(tangent.x() * tangent.x() + tangent.y() * tangent.y());
        if (len > 0.001) {
            tangent /= len;
        }

        // Calculate handle distance based on neighbors
        int prevIdx = (vertexIndex - 1 + n) % n;
        int nextIdx = (vertexIndex + 1) % n;
        QPointF p0 = vertices[prevIdx].position;
        QPointF p2 = vertices[nextIdx].position;
        double dist = std::sqrt((p2 - p0).x() * (p2 - p0).x() + (p2 - p0).y() * (p2 - p0).y());

        if (side < 0) {
            // Incoming handle (opposite direction)
            double handleDist = (dist / 6.0) * vertex.incomingTension;
            return vertexPos - tangent * handleDist;
        } else {
            // Outgoing handle
            double handleDist = (dist / 6.0) * vertex.outgoingTension;
            return vertexPos + tangent * handleDist;
        }
    } else {
        // Use Catmull-Rom default
        int prevIdx = (vertexIndex - 1 + n) % n;
        int nextIdx = (vertexIndex + 1) % n;
        QPointF p0 = vertices[prevIdx].position;
        QPointF p2 = vertices[nextIdx].position;

        if (!polyline->isClosed() && vertexIndex == 0) p0 = vertexPos;
        if (!polyline->isClosed() && vertexIndex == n - 1) p2 = vertexPos;

        QPointF tangent = (p2 - p0);
        double len = std::sqrt(tangent.x() * tangent.x() + tangent.y() * tangent.y());
        if (len > 0.001) {
            tangent /= len;
        }

        if (side < 0) {
            double handleDist = (len / 6.0) * vertex.incomingTension;
            return vertexPos - tangent * handleDist;
        } else {
            double handleDist = (len / 6.0) * vertex.outgoingTension;
            return vertexPos + tangent * handleDist;
        }
    }
}

int SelectTool::findHandleAt(const QPointF& point, Geometry::GeometryObject** outObject, int* outSide) const
{
    if (!m_document) {
        return -1;
    }

    const double tolerance = 8.0; // pixels

    // Check selected or hovered vertices first
    if (m_selectedVertexObject) {
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            auto vertices = polyline->vertices();
            if (m_selectedVertexIndex >= 0 && m_selectedVertexIndex < vertices.size()) {
                if (vertices[m_selectedVertexIndex].type == Geometry::VertexType::Smooth) {
                    // Check incoming handle
                    QPointF handlePos = getHandlePosition(polyline, m_selectedVertexIndex, -1);
                    QPointF delta = point - handlePos;
                    double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
                    if (dist <= tolerance) {
                        if (outObject) *outObject = m_selectedVertexObject;
                        if (outSide) *outSide = -1;
                        return m_selectedVertexIndex;
                    }

                    // Check outgoing handle
                    handlePos = getHandlePosition(polyline, m_selectedVertexIndex, +1);
                    delta = point - handlePos;
                    dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
                    if (dist <= tolerance) {
                        if (outObject) *outObject = m_selectedVertexObject;
                        if (outSide) *outSide = +1;
                        return m_selectedVertexIndex;
                    }
                }
            }
        }
    }

    // Check all objects for handles near hovered smooth vertices
    for (auto* obj : m_document->objects()) {
        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
            auto vertices = polyline->vertices();
            for (int i = 0; i < vertices.size(); ++i) {
                if (vertices[i].type == Geometry::VertexType::Smooth) {
                    // Check incoming handle
                    QPointF handlePos = getHandlePosition(polyline, i, -1);
                    QPointF delta = point - handlePos;
                    double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
                    if (dist <= tolerance) {
                        if (outObject) *outObject = obj;
                        if (outSide) *outSide = -1;
                        return i;
                    }

                    // Check outgoing handle
                    handlePos = getHandlePosition(polyline, i, +1);
                    delta = point - handlePos;
                    dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
                    if (dist <= tolerance) {
                        if (outObject) *outObject = obj;
                        if (outSide) *outSide = +1;
                        return i;
                    }
                }
            }
        }
    }

    return -1;
}

void SelectTool::drawCurveHandles(QPainter* painter, Geometry::GeometryObject* obj, int vertexIndex) const
{
    auto* polyline = dynamic_cast<Geometry::Polyline*>(obj);
    if (!polyline) return;

    auto vertices = polyline->vertices();
    if (vertexIndex < 0 || vertexIndex >= vertices.size()) return;

    const Geometry::PolylineVertex& vertex = vertices[vertexIndex];
    if (vertex.type != Geometry::VertexType::Smooth) return;

    QPointF vertexPos = vertex.position;

    // Get handle positions
    QPointF inHandle = getHandlePosition(polyline, vertexIndex, -1);
    QPointF outHandle = getHandlePosition(polyline, vertexIndex, +1);

    painter->save();

    // Draw handle lines
    QPen handleLinePen(QColor(100, 100, 255), 1, Qt::SolidLine);
    painter->setPen(handleLinePen);
    painter->drawLine(vertexPos, inHandle);
    painter->drawLine(vertexPos, outHandle);

    // Draw handle circles
    const double handleSize = 5.0;
    const double hoveredHandleSize = 7.0;

    bool inHovered = (m_hoveredHandleVertexIndex == vertexIndex && m_hoveredHandleSide == -1);
    bool outHovered = (m_hoveredHandleVertexIndex == vertexIndex && m_hoveredHandleSide == +1);
    bool inSelected = (m_selectedHandleVertexIndex == vertexIndex && m_selectedHandleSide == -1 && m_mode == SelectMode::DraggingHandle);
    bool outSelected = (m_selectedHandleVertexIndex == vertexIndex && m_selectedHandleSide == +1 && m_mode == SelectMode::DraggingHandle);

    // Incoming handle
    double inSize = (inHovered || inSelected) ? hoveredHandleSize : handleSize;
    QColor inColor = inSelected ? QColor(255, 100, 0) : (inHovered ? QColor(255, 165, 0) : QColor(100, 100, 255));
    painter->setPen(QPen(inColor, 2));
    painter->setBrush(QBrush(Qt::white));
    painter->drawEllipse(inHandle, inSize, inSize);

    // Outgoing handle
    double outSize = (outHovered || outSelected) ? hoveredHandleSize : handleSize;
    QColor outColor = outSelected ? QColor(255, 100, 0) : (outHovered ? QColor(255, 165, 0) : QColor(100, 100, 255));
    painter->setPen(QPen(outColor, 2));
    painter->setBrush(QBrush(Qt::white));
    painter->drawEllipse(outHandle, outSize, outSize);

    painter->restore();
}

// ============================================================================
// Multi-Segment Lock Implementation
// ============================================================================

void SelectTool::toggleSegmentSelection(Geometry::GeometryObject* obj, int segmentIdx)
{
    if (!obj) return;

    SegmentKey key{obj, segmentIdx};

    if (m_selectedSegments.contains(key)) {
        m_selectedSegments.remove(key);
        showStatusMessage(tr("Segment deselected"));
    } else {
        m_selectedSegments.insert(key);
        showStatusMessage(tr("Segment selected (press L to lock %1 segments)")
            .arg(m_selectedSegments.size()));
    }

    if (m_canvas) {
        m_canvas->viewport()->update();
    }
}

void SelectTool::lockSelectedSegments()
{
    if (m_selectedSegments.isEmpty()) {
        showStatusMessage(tr("No segments selected to lock"));
        return;
    }

    // Lock all selected segments
    for (const auto& key : m_selectedSegments) {
        Geometry::Polyline* polyline = dynamic_cast<Geometry::Polyline*>(key.object);
        if (polyline) {
            // Calculate and store segment length
            double length = polyline->calculateSegmentLength(key.segmentIndex);
            m_lockedLengths[key] = length;

            // Store segment positions
            auto vertices = polyline->vertices();
            int n = vertices.size();
            QPointF start = vertices[key.segmentIndex].position;
            QPointF end = vertices[(key.segmentIndex + 1) % n].position;
            m_lockedSegmentPositions[key] = qMakePair(start, end);

            m_lockedSegments.insert(key);
        }
    }

    showStatusMessage(tr("🔒 %1 segment(s) locked").arg(m_lockedSegments.size()));

    // Clear selection
    m_selectedSegments.clear();

    if (m_canvas) {
        m_canvas->viewport()->update();
    }
}

void SelectTool::unlockSegment(Geometry::GeometryObject* obj, int segmentIdx)
{
    SegmentKey key{obj, segmentIdx};

    if (m_lockedSegments.contains(key)) {
        m_lockedSegments.remove(key);
        m_lockedLengths.remove(key);
        m_lockedSegmentPositions.remove(key);
        showStatusMessage(tr("Segment unlocked"));

        if (m_canvas) {
            m_canvas->viewport()->update();
        }
    }
}

void SelectTool::unlockAllSegments()
{
    int count = m_lockedSegments.size();
    m_lockedSegments.clear();
    m_lockedLengths.clear();
    m_lockedSegmentPositions.clear();
    m_selectedSegments.clear();

    showStatusMessage(tr("All %1 segment(s) unlocked").arg(count));

    if (m_canvas) {
        m_canvas->viewport()->update();
    }
}

bool SelectTool::isSegmentLocked(Geometry::GeometryObject* obj, int segmentIdx) const
{
    SegmentKey key{const_cast<Geometry::GeometryObject*>(obj), segmentIdx};
    return m_lockedSegments.contains(key);
}

bool SelectTool::isSegmentSelected(Geometry::GeometryObject* obj, int segmentIdx) const
{
    SegmentKey key{const_cast<Geometry::GeometryObject*>(obj), segmentIdx};
    return m_selectedSegments.contains(key);
}

void SelectTool::drawSelectedSegments(QPainter* painter) const
{
    if (!painter) return;

    painter->save();
    painter->setPen(QPen(QColor(255, 165, 0), 3, Qt::DashLine)); // Orange, dashed
    painter->setBrush(Qt::NoBrush);

    for (const auto& key : m_selectedSegments) {
        Geometry::Polyline* polyline = dynamic_cast<Geometry::Polyline*>(key.object);
        if (polyline) {
            auto vertices = polyline->vertices();
            int n = vertices.size();
            int i = key.segmentIndex;
            int nextIdx = (i + 1) % n;

            const Geometry::PolylineVertex& current = vertices[i];
            const Geometry::PolylineVertex& next = vertices[nextIdx];

            // A segment is curved if EITHER endpoint is smooth
            bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                              (next.type == Geometry::VertexType::Smooth);

            QPainterPath path;
            path.moveTo(current.position);

            if (needsCurve) {
                QPointF p1 = current.position;
                QPointF p2 = next.position;

                // Distance between points for scaling control points
                QPointF segment = p2 - p1;
                double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
                double controlDistance = dist / 3.0;

                // Calculate control points for cubic Bezier
                QPointF c1, c2;

                // Determine control point c1 (outgoing from current)
                if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
                    c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
                } else if (current.type == Geometry::VertexType::Smooth) {
                    int prevIdx = (i - 1 + n) % n;
                    QPointF p0 = vertices[prevIdx].position;
                    c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
                } else {
                    c1 = p1 + (p2 - p1) * 0.01;
                }

                // Determine control point c2 (incoming to next)
                if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
                    c2 = p2 - next.tangent * controlDistance * next.incomingTension;
                } else if (next.type == Geometry::VertexType::Smooth) {
                    int nextNextIdx = (i + 2) % n;
                    QPointF p3 = vertices[nextNextIdx].position;
                    c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
                } else {
                    c2 = p2 - (p2 - p1) * 0.01;
                }

                path.cubicTo(c1, c2, p2);
            } else {
                // Both endpoints are sharp: straight line
                path.lineTo(next.position);
            }

            painter->strokePath(path, painter->pen());
        }
    }

    painter->restore();
}

void SelectTool::drawLockedSegments(QPainter* painter) const
{
    if (!painter) return;

    painter->save();
    painter->setPen(QPen(QColor(220, 20, 20), 4)); // Red, thick
    painter->setBrush(Qt::NoBrush);

    for (const auto& key : m_lockedSegments) {
        Geometry::Polyline* polyline = dynamic_cast<Geometry::Polyline*>(key.object);
        if (polyline) {
            auto vertices = polyline->vertices();
            int n = vertices.size();
            int i = key.segmentIndex;
            int nextIdx = (i + 1) % n;

            const Geometry::PolylineVertex& current = vertices[i];
            const Geometry::PolylineVertex& next = vertices[nextIdx];

            // A segment is curved if EITHER endpoint is smooth
            bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                              (next.type == Geometry::VertexType::Smooth);

            QPainterPath path;
            path.moveTo(current.position);

            if (needsCurve) {
                QPointF p1 = current.position;
                QPointF p2 = next.position;

                // Distance between points for scaling control points
                QPointF segment = p2 - p1;
                double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
                double controlDistance = dist / 3.0;

                // Calculate control points for cubic Bezier
                QPointF c1, c2;

                // Determine control point c1 (outgoing from current)
                if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
                    c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
                } else if (current.type == Geometry::VertexType::Smooth) {
                    int prevIdx = (i - 1 + n) % n;
                    QPointF p0 = vertices[prevIdx].position;
                    c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
                } else {
                    c1 = p1 + (p2 - p1) * 0.01;
                }

                // Determine control point c2 (incoming to next)
                if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
                    c2 = p2 - next.tangent * controlDistance * next.incomingTension;
                } else if (next.type == Geometry::VertexType::Smooth) {
                    int nextNextIdx = (i + 2) % n;
                    QPointF p3 = vertices[nextNextIdx].position;
                    c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
                } else {
                    c2 = p2 - (p2 - p1) * 0.01;
                }

                path.cubicTo(c1, c2, p2);
            } else {
                // Both endpoints are sharp: straight line
                path.lineTo(next.position);
            }

            // Draw the path (stroke only, no fill)
            QPen curvePen(QColor(220, 20, 20), 4);
            painter->strokePath(path, curvePen);

            // Draw lock icon at midpoint of the curve
            QPointF midpoint = path.pointAtPercent(0.5);
            painter->setPen(QPen(Qt::white, 1));
            painter->setBrush(QBrush(QColor(220, 20, 20)));

            // Simple lock icon: rectangle with arc
            QRectF lockRect(midpoint.x() - 6, midpoint.y() - 4, 12, 8);
            painter->drawRect(lockRect);
            painter->setBrush(Qt::NoBrush);
            painter->drawArc(QRectF(midpoint.x() - 4, midpoint.y() - 8, 8, 8), 0, 180 * 16);

            // Draw length text
            if (m_lockedLengths.contains(key)) {
                double length = m_lockedLengths[key];
                QString text = QString("%1").arg(length, 0, 'f', 1);
                QRectF textRect(midpoint.x() - 20, midpoint.y() + 10, 40, 15);
                painter->setPen(Qt::black);
                painter->setBrush(QBrush(QColor(255, 255, 255, 200)));
                painter->drawRect(textRect);
                painter->drawText(textRect, Qt::AlignCenter, text);
            }
        }
    }

    painter->restore();
}

QPointF SelectTool::constrainVertexWithLockedSegments(const QPointF& newPos, Geometry::Polyline* polyline, int vertexIndex) const
{
    if (!polyline) return newPos;

    auto vertices = polyline->vertices();
    int n = vertices.size();
    int prevSegment = (vertexIndex - 1 + n) % n;
    int nextSegment = vertexIndex;

    SegmentKey prevKey{polyline, prevSegment};
    SegmentKey nextKey{polyline, nextSegment};

    bool prevLocked = m_lockedSegments.contains(prevKey);
    bool nextLocked = m_lockedSegments.contains(nextKey);

    // Case 1: Both adjacent segments locked → vertex cannot move
    if (prevLocked && nextLocked) {
        return vertices[vertexIndex].position; // Return original position
    }

    // Case 2: Only previous segment locked → pivot around previous vertex
    if (prevLocked) {
        double lockedLength = m_lockedLengths.value(prevKey, 0.0);

        // For curved segments, we need to iteratively find the position that maintains curve length
        // Check if this segment is curved
        const Geometry::PolylineVertex& current = vertices[(vertexIndex - 1 + n) % n];
        const Geometry::PolylineVertex& next = vertices[vertexIndex];
        bool isCurved = (current.type == Geometry::VertexType::Smooth) ||
                        (next.type == Geometry::VertexType::Smooth);

        if (isCurved) {
            // Iteratively adjust position to maintain curve length
            QPointF testPos = newPos;
            const int maxIterations = 10;

            for (int iter = 0; iter < maxIterations; ++iter) {
                // Temporarily update the vertex position to test
                double currentLength = polyline->calculateSegmentLength(prevSegment, vertexIndex, testPos);
                double error = currentLength - lockedLength;

                if (std::abs(error) < 0.5) {
                    return testPos; // Close enough
                }

                // Adjust position: move closer or farther from pivot
                QPointF pivot = vertices[(vertexIndex - 1 + n) % n].position;
                QPointF delta = testPos - pivot;
                double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (dist > 0.001) {
                    // Scale the distance based on error (curve is longer → move closer)
                    double adjustment = -error * 0.5;
                    testPos = pivot + (delta / dist) * (dist + adjustment);
                } else {
                    break;
                }
            }
            return testPos;
        } else {
            // Straight segment: simple circle constraint
            QPointF pivot = vertices[(vertexIndex - 1 + n) % n].position;
            QPointF delta = newPos - pivot;
            double currentDist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
            if (currentDist > 0.001) {
                QPointF constrained = pivot + (delta / currentDist) * lockedLength;
                return constrained;
            }
            return vertices[vertexIndex].position;
        }
    }

    // Case 3: Only next segment locked → pivot around next vertex
    if (nextLocked) {
        double lockedLength = m_lockedLengths.value(nextKey, 0.0);

        // For curved segments, we need to iteratively find the position that maintains curve length
        // Check if this segment is curved
        const Geometry::PolylineVertex& current = vertices[vertexIndex];
        const Geometry::PolylineVertex& next = vertices[(vertexIndex + 1) % n];
        bool isCurved = (current.type == Geometry::VertexType::Smooth) ||
                        (next.type == Geometry::VertexType::Smooth);

        if (isCurved) {
            // Iteratively adjust position to maintain curve length
            QPointF testPos = newPos;
            const int maxIterations = 10;

            for (int iter = 0; iter < maxIterations; ++iter) {
                // Temporarily update the vertex position to test
                double currentLength = polyline->calculateSegmentLength(nextSegment, vertexIndex, testPos);
                double error = currentLength - lockedLength;

                if (std::abs(error) < 0.5) {
                    return testPos; // Close enough
                }

                // Adjust position: move closer or farther from pivot
                QPointF pivot = vertices[(vertexIndex + 1) % n].position;
                QPointF delta = testPos - pivot;
                double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

                if (dist > 0.001) {
                    // Scale the distance based on error (curve is longer → move closer)
                    double adjustment = -error * 0.5;
                    testPos = pivot + (delta / dist) * (dist + adjustment);
                } else {
                    break;
                }
            }
            return testPos;
        } else {
            // Straight segment: simple circle constraint
            QPointF pivot = vertices[(vertexIndex + 1) % n].position;
            QPointF delta = newPos - pivot;
            double currentDist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
            if (currentDist > 0.001) {
                QPointF constrained = pivot + (delta / currentDist) * lockedLength;
                return constrained;
            }
            return vertices[vertexIndex].position;
        }
    }

    // Case 4: No locked segments → free movement
    return newPos;
}

bool SelectTool::hasVertexTargeted() const
{
    // Check if a vertex is selected or hovered
    if ((m_mode == SelectMode::DraggingVertex || m_mode == SelectMode::VertexSelected) && m_selectedVertexObject) {
        return true;
    }
    if (m_hoveredVertexIndex >= 0) {
        return true;
    }
    return false;
}

void SelectTool::updateStatusMessage()
{
    if (!m_document) {
        return;
    }

    auto selected = m_document->selectedObjects();

    if (m_mode == SelectMode::VertexSelected) {
        showStatusMessage("Vertex selected | Drag=Move | Tab=Enter length | C=Convert Sharp/Smooth | Del=Delete | Click elsewhere=Deselect");
    } else if (!selected.isEmpty()) {
        // Objects selected
        showStatusMessage("Selection: Rotate (R) | Mirror (M) | Scale (S) | Delete (Del) | Right-click for menu");
    } else {
        // No selection
        showStatusMessage("Select Tool: Click object or drag box to select | Click vertex to edit | Ctrl+Click for multi-select");
    }
}

} // namespace Tools
} // namespace PatternCAD
