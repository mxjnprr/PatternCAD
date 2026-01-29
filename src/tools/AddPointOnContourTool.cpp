/**
 * AddPointOnContourTool.cpp
 *
 * Implementation of AddPointOnContourTool
 */

#include "AddPointOnContourTool.h"
#include "core/Document.h"
#include "core/Commands.h"
#include "geometry/GeometryObject.h"
#include "geometry/Line.h"
#include "geometry/Polyline.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

namespace PatternCAD {
namespace Tools {

AddPointOnContourTool::AddPointOnContourTool(QObject* parent)
    : Tool(parent)
    , m_hoveredObject(nullptr)
    , m_hasProjection(false)
    , m_mode(EditMode::AddingPoint)
    , m_selectedVertexObject(nullptr)
    , m_selectedVertexIndex(-1)
    , m_hoveredVertexIndex(-1)
{
}

AddPointOnContourTool::~AddPointOnContourTool()
{
}

QString AddPointOnContourTool::name() const
{
    return "Add Point on Contour";
}

QString AddPointOnContourTool::description() const
{
    return "Click on contour to add point, click vertex to move it. T=toggle type, Del=delete vertex";
}

QCursor AddPointOnContourTool::cursor() const
{
    return QCursor(Qt::CrossCursor);
}

void AddPointOnContourTool::activate()
{
    Tool::activate();
    m_hoveredObject = nullptr;
    m_hasProjection = false;
    m_mode = EditMode::AddingPoint;
    m_selectedVertexObject = nullptr;
    m_selectedVertexIndex = -1;
    m_hoveredVertexIndex = -1;
    showStatusMessage("Click on contour to add point, click vertex to move it. T=toggle type, Del=delete");
}

void AddPointOnContourTool::reset()
{
    Tool::reset();
    m_hoveredObject = nullptr;
    m_hasProjection = false;
    m_mode = EditMode::AddingPoint;
    m_selectedVertexObject = nullptr;
    m_selectedVertexIndex = -1;
    m_hoveredVertexIndex = -1;
}

void AddPointOnContourTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) {
        return;
    }

    m_currentPoint = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        // Check if clicking on a vertex first
        Geometry::GeometryObject* vertexObject = nullptr;
        int vertexIndex = findVertexAt(m_currentPoint, &vertexObject);

        if (vertexIndex >= 0) {
            // Start dragging vertex
            m_selectedVertexObject = vertexObject;
            m_selectedVertexIndex = vertexIndex;
            m_mode = EditMode::DraggingVertex;
            m_lastPoint = m_currentPoint;
            showStatusMessage(QString("Dragging vertex %1 (T=toggle type, Del=delete)").arg(vertexIndex + 1));
        } else if (m_hasProjection) {
            // Add new point to contour
            Geometry::GeometryObject* obj = m_hoveredObject;

            if (auto* line = dynamic_cast<Geometry::Line*>(obj)) {
                addPointToLine(line, m_projectedPoint);
            } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
                addPointToPolyline(polyline, m_projectedPoint);
            }
        }
    }

    event->accept();
}

void AddPointOnContourTool::mouseMoveEvent(QMouseEvent* event)
{
    QPointF oldPoint = m_currentPoint;
    m_currentPoint = mapToScene(event->pos());

    if (m_mode == EditMode::DraggingVertex) {
        // Move vertex
        QPointF delta = m_currentPoint - m_lastPoint;

        if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
            Geometry::PolylineVertex vertex = polyline->vertexAt(m_selectedVertexIndex);
            polyline->updateVertex(m_selectedVertexIndex, vertex.position + delta);
        } else if (auto* line = dynamic_cast<Geometry::Line*>(m_selectedVertexObject)) {
            // For lines, move start or end point
            if (m_selectedVertexIndex == 0) {
                line->setStart(line->start() + delta);
            } else {
                line->setEnd(line->end() + delta);
            }
        }

        m_lastPoint = m_currentPoint;
    } else {
        // Check for hovered vertex first
        Geometry::GeometryObject* vertexObject = nullptr;
        m_hoveredVertexIndex = findVertexAt(m_currentPoint, &vertexObject);

        if (m_hoveredVertexIndex >= 0) {
            showStatusMessage(QString("Click to move vertex %1 (T=toggle type, Del=delete)").arg(m_hoveredVertexIndex + 1));
            m_hoveredObject = vertexObject;
            m_hasProjection = false;
        } else {
            // Find line or polyline under cursor
            m_hoveredObject = findLineOrPolylineAt(m_currentPoint);
            m_hasProjection = false;

            if (m_hoveredObject) {
                if (auto* line = dynamic_cast<Geometry::Line*>(m_hoveredObject)) {
                    m_projectedPoint = line->closestPointOnLine(m_currentPoint);
                    m_hasProjection = true;
                    showStatusMessage(QString("Click to add point on line"));
                } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_hoveredObject)) {
                    int segmentIdx = polyline->findClosestSegment(m_currentPoint, &m_projectedPoint);
                    m_hasProjection = (segmentIdx >= 0);
                    if (m_hasProjection) {
                        showStatusMessage(QString("Click to add point on polyline (segment %1)").arg(segmentIdx + 1));
                    }
                }
            } else {
                showStatusMessage("Move cursor over a contour or vertex");
            }
        }
    }

    event->accept();
}

void AddPointOnContourTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_mode == EditMode::DraggingVertex) {
        // Finish vertex drag
        showStatusMessage("Vertex moved (T=toggle type, Del=delete)");
        m_mode = EditMode::AddingPoint;
        // Keep vertex selected for further editing
    }

    event->accept();
}

void AddPointOnContourTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_T) {
        // Toggle vertex type (Sharp <-> Smooth)
        toggleVertexType();
        event->accept();
    } else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        // Delete selected vertex
        deleteSelectedVertex();
        event->accept();
    } else if (event->key() == Qt::Key_Escape) {
        // Deselect vertex
        m_selectedVertexObject = nullptr;
        m_selectedVertexIndex = -1;
        m_mode = EditMode::AddingPoint;
        showStatusMessage("Vertex deselected");
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void AddPointOnContourTool::drawOverlay(QPainter* painter)
{
    painter->save();

    // Draw highlight on hovered object
    if (m_hoveredObject) {
        QPen highlightPen(QColor(255, 165, 0), 3, Qt::SolidLine); // Orange
        painter->setPen(highlightPen);
        painter->setBrush(Qt::NoBrush);

        if (auto* line = dynamic_cast<Geometry::Line*>(m_hoveredObject)) {
            painter->drawLine(line->start(), line->end());
        } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_hoveredObject)) {
            polyline->draw(painter, QColor(255, 165, 0));
        }

        // Draw vertex handles for hovered object
        drawVertexHandles(painter, m_hoveredObject);
    }

    // Draw projection point (for adding new point)
    if (m_hasProjection && m_mode == EditMode::AddingPoint) {
        QPen pointPen(Qt::red, 2);
        QBrush pointBrush(Qt::red);
        painter->setPen(pointPen);
        painter->setBrush(pointBrush);

        const double radius = 4.0;
        painter->drawEllipse(m_projectedPoint, radius, radius);

        // Draw line from cursor to projection
        QPen guidePen(Qt::red, 1, Qt::DashLine);
        painter->setPen(guidePen);
        painter->drawLine(m_currentPoint, m_projectedPoint);
    }

    painter->restore();
}

Geometry::GeometryObject* AddPointOnContourTool::findLineOrPolylineAt(const QPointF& point)
{
    if (!m_document) {
        return nullptr;
    }

    const double tolerance = 10.0;

    // Search in reverse order (top to bottom)
    auto objects = m_document->objects();
    for (int i = objects.size() - 1; i >= 0; --i) {
        Geometry::GeometryObject* obj = objects[i];

        if (auto* line = dynamic_cast<Geometry::Line*>(obj)) {
            if (line->distanceToPoint(point) <= tolerance) {
                return line;
            }
        } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
            QPointF closestPoint;
            int segmentIdx = polyline->findClosestSegment(point, &closestPoint);
            if (segmentIdx >= 0) {
                QPointF delta = point - closestPoint;
                double dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
                if (dist <= tolerance) {
                    return polyline;
                }
            }
        }
    }

    return nullptr;
}

void AddPointOnContourTool::addPointToLine(Geometry::Line* line, const QPointF& point)
{
    // Convert Line to Polyline with 3 vertices
    QVector<Geometry::PolylineVertex> vertices;
    vertices.append(Geometry::PolylineVertex(line->start(), Geometry::VertexType::Sharp));
    vertices.append(Geometry::PolylineVertex(point, Geometry::VertexType::Sharp));
    vertices.append(Geometry::PolylineVertex(line->end(), Geometry::VertexType::Sharp));

    auto* polyline = new Geometry::Polyline(vertices);
    polyline->setLayer(line->layer());
    polyline->setVisible(line->isVisible());
    polyline->setLocked(line->isLocked());
    polyline->setClosed(false);

    // Remove old line and add new polyline
    m_document->removeObject(line);
    m_document->addObject(polyline);

    showStatusMessage("Point added - Line converted to polyline");
}

void AddPointOnContourTool::addPointToPolyline(Geometry::Polyline* polyline, const QPointF& point)
{
    // Find which segment the point is on
    int segmentIdx = polyline->findClosestSegment(point);

    if (segmentIdx >= 0) {
        // Insert new vertex after the segment start
        Geometry::PolylineVertex newVertex(point, Geometry::VertexType::Sharp);
        polyline->insertVertex(segmentIdx + 1, newVertex);

        // Select the newly added vertex
        m_selectedVertexObject = polyline;
        m_selectedVertexIndex = segmentIdx + 1;

        showStatusMessage(QString("Point added at segment %1 (T=toggle type, Del=delete)").arg(segmentIdx + 1));
    }
}

int AddPointOnContourTool::findVertexAt(const QPointF& point, Geometry::GeometryObject** outObject) const
{
    if (!m_document) {
        return -1;
    }

    const double tolerance = 8.0;

    // Check all objects
    auto objects = m_document->objects();
    for (auto* obj : objects) {
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

void AddPointOnContourTool::drawVertexHandles(QPainter* painter, Geometry::GeometryObject* obj) const
{
    if (!obj) return;

    const double handleSize = 6.0;
    const double hoveredHandleSize = 8.0;

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(obj)) {
        auto vertices = polyline->vertices();
        for (int i = 0; i < vertices.size(); ++i) {
            QPointF pos = vertices[i].position;
            bool isHovered = (m_hoveredVertexIndex == i && m_hoveredObject == obj);
            bool isSelected = (m_selectedVertexObject == obj && m_selectedVertexIndex == i);

            // Choose color based on state and vertex type
            QColor color;
            if (isSelected) {
                color = QColor(0, 255, 0); // Green for selected
            } else if (isHovered) {
                color = QColor(255, 165, 0); // Orange for hovered
            } else if (vertices[i].type == Geometry::VertexType::Smooth) {
                color = QColor(100, 149, 237); // Cornflower blue for smooth
            } else {
                color = Qt::blue; // Blue for sharp
            }

            double size = (isHovered || isSelected) ? hoveredHandleSize : handleSize;

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
            bool isHovered = (m_hoveredVertexIndex == i && m_hoveredObject == obj);
            bool isSelected = (m_selectedVertexObject == obj && m_selectedVertexIndex == i);

            QColor color;
            if (isSelected) {
                color = QColor(0, 255, 0); // Green
            } else if (isHovered) {
                color = QColor(255, 165, 0); // Orange
            } else {
                color = Qt::blue;
            }

            double size = (isHovered || isSelected) ? hoveredHandleSize : handleSize;

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

void AddPointOnContourTool::toggleVertexType()
{
    if (m_selectedVertexIndex < 0 || !m_selectedVertexObject) {
        showStatusMessage("No vertex selected");
        return;
    }

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
        auto vertices = polyline->vertices();
        if (m_selectedVertexIndex >= 0 && m_selectedVertexIndex < vertices.size()) {
            Geometry::PolylineVertex vertex = vertices[m_selectedVertexIndex];

            // Toggle type
            if (vertex.type == Geometry::VertexType::Sharp) {
                vertex.type = Geometry::VertexType::Smooth;
                showStatusMessage(QString("Vertex %1 is now Smooth (curve)").arg(m_selectedVertexIndex + 1));
            } else {
                vertex.type = Geometry::VertexType::Sharp;
                showStatusMessage(QString("Vertex %1 is now Sharp (corner)").arg(m_selectedVertexIndex + 1));
            }

            // Update the vertex
            vertices[m_selectedVertexIndex] = vertex;
            polyline->setVertices(vertices);
        }
    } else {
        showStatusMessage("Type toggle only works on polylines");
    }
}

void AddPointOnContourTool::deleteSelectedVertex()
{
    if (m_selectedVertexIndex < 0 || !m_selectedVertexObject) {
        showStatusMessage("No vertex selected");
        return;
    }

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_selectedVertexObject)) {
        int vertexCount = polyline->vertexCount();
        if (vertexCount <= 3) {
            showStatusMessage("Cannot delete - polyline must have at least 3 vertices");
            return;
        }

        polyline->removeVertex(m_selectedVertexIndex);
        showStatusMessage(QString("Vertex %1 deleted").arg(m_selectedVertexIndex + 1));

        // Deselect
        m_selectedVertexObject = nullptr;
        m_selectedVertexIndex = -1;
    } else if (auto* line = dynamic_cast<Geometry::Line*>(m_selectedVertexObject)) {
        showStatusMessage("Cannot delete line vertices - use Delete on the whole line");
    }
}

} // namespace Tools
} // namespace PatternCAD
