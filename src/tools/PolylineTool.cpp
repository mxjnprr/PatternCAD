/**
 * PolylineTool.cpp
 *
 * Implementation of PolylineTool
 */

#include "PolylineTool.h"
#include "core/Document.h"
#include "core/Units.h"
#include "geometry/Polyline.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QDebug>
#include <cmath>

namespace PatternCAD {
namespace Tools {

PolylineTool::PolylineTool(QObject* parent)
    : Tool(parent)
    , m_isDrawing(false)
    , m_shiftPressed(false)
    , m_isDragging(false)
{
}

PolylineTool::~PolylineTool()
{
}

QString PolylineTool::name() const
{
    return "Polyline";
}

QString PolylineTool::description() const
{
    return "Click for sharp point, Shift+Click for smooth curve";
}

void PolylineTool::activate()
{
    Tool::activate();
    m_isDrawing = false;
    m_vertices.clear();
    showStatusMessage("Click to start polyline (Shift for smooth point)");
}

void PolylineTool::reset()
{
    Tool::reset();
    m_isDrawing = false;
    m_vertices.clear();
    showStatusMessage("Click to start polyline (Shift for smooth point)");
}

void PolylineTool::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPointF point = mapToScene(event->pos());
    point = snapToGrid(point);

    // Store press point to detect drag
    m_pressPoint = point;
    m_isDragging = false;

    event->accept();
}

void PolylineTool::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = mapToScene(event->pos());
    point = snapToGrid(point);
    m_currentPoint = point;

    // Detect Shift key state for preview
    m_shiftPressed = (event->modifiers() & Qt::ShiftModifier);

    // Check if we're dragging (mouse pressed and moved)
    if (event->buttons() & Qt::LeftButton) {
        QPointF delta = m_currentPoint - m_pressPoint;
        double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        if (distance > 5.0) {  // Threshold to distinguish click from drag
            m_isDragging = true;
        }
    }

    event->accept();
}

void PolylineTool::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPointF point = mapToScene(event->pos());
    point = snapToGrid(point);

    if (m_isDragging) {
        // Drag detected: create smooth vertex with tension based on drag distance
        QPointF dragVector = point - m_pressPoint;
        double dragDistance = std::sqrt(dragVector.x() * dragVector.x() + dragVector.y() * dragVector.y());

        // Map drag distance to tension (0.1 to 1.0)
        // Short drag = subtle curve, long drag = pronounced curve
        double tension = qBound(0.1, dragDistance / 100.0, 1.0);

        // Normalize the drag vector to get the tangent direction
        QPointF tangent = dragVector;
        if (dragDistance > 0.01) {
            tangent = dragVector / dragDistance; // Normalize to unit vector
        }

        addVertex(m_pressPoint, Geometry::VertexType::Smooth, tension, tangent);
        m_isDragging = false;
    } else {
        // Simple click: create sharp vertex
        addVertex(point, Geometry::VertexType::Sharp, 0.5, QPointF());
    }

    event->accept();
}

void PolylineTool::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isDrawing) {
        finishPolyline();
        event->accept();
    }
}

void PolylineTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (m_isDrawing && m_vertices.size() >= 3) {
            finishPolyline();
            event->accept();
            return;
        }
    } else if (event->key() == Qt::Key_Escape) {
        cancelPolyline();
        event->accept();
        return;
    } else if (event->key() == Qt::Key_Tab && m_isDrawing) {
        // Calculate current length and angle for pre-filling the dialog
        double currentLength = 0.0;
        double currentAngle = 0.0;

        if (!m_vertices.isEmpty()) {
            QPointF lastPoint = m_vertices.last().position;
            QPointF delta = m_currentPoint - lastPoint;
            currentLength = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
            currentAngle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;
        }

        // Request dimension input for segment length with current values
        qDebug() << "PolylineTool: Tab pressed, emitting dimensionInputRequested with length:" << currentLength << "angle:" << currentAngle;
        emit dimensionInputRequested("length", currentLength, currentAngle);
        event->accept();
        return;
    }

    Tool::keyPressEvent(event);
}

void PolylineTool::drawOverlay(QPainter* painter)
{
    if (m_vertices.isEmpty()) {
        return;
    }

    painter->save();

    // Create temporary vertices including the point being dragged
    QVector<Geometry::PolylineVertex> tempVertices = m_vertices;

    if (m_isDragging) {
        // Add the dragged point with its tension to the preview
        QPointF dragVector = m_currentPoint - m_pressPoint;
        double dragDistance = std::sqrt(dragVector.x() * dragVector.x() + dragVector.y() * dragVector.y());
        double tension = qBound(0.1, dragDistance / 100.0, 1.0);

        // Calculate normalized tangent
        QPointF tangent = dragVector;
        if (dragDistance > 0.01) {
            tangent = dragVector / dragDistance;
        }

        tempVertices.append(Geometry::PolylineVertex(m_pressPoint,
                                                     Geometry::VertexType::Smooth,
                                                     tension,
                                                     tangent));
    } else if (m_isDrawing && !m_isDragging) {
        // Just add current point as preview
        tempVertices.append(Geometry::PolylineVertex(m_currentPoint,
                                                     Geometry::VertexType::Sharp,
                                                     0.5,
                                                     QPointF()));
    }

    // Draw preview path with the temporary point included
    if (tempVertices.size() >= 2) {
        QPainterPath previewPath;
        int n = tempVertices.size();

        previewPath.moveTo(tempVertices[0].position);

        for (int i = 0; i < n - 1; ++i) {
            const Geometry::PolylineVertex& current = tempVertices[i];
            const Geometry::PolylineVertex& next = tempVertices[i + 1];

            bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                              (next.type == Geometry::VertexType::Smooth);

            if (needsCurve) {
                QPointF p1 = current.position;
                QPointF p2 = next.position;

                // Distance between points
                QPointF segment = p2 - p1;
                double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
                double controlDistance = dist / 3.0;

                // Calculate control points
                QPointF c1, c2;

                // Outgoing control point from current vertex
                if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
                    c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
                } else {
                    QPointF p0 = (i > 0) ? tempVertices[i - 1].position : current.position;
                    double tension = (current.type == Geometry::VertexType::Smooth) ? current.outgoingTension : 0.5;
                    c1 = p1 + (p2 - p0) * (tension / 3.0);
                }

                // Incoming control point to next vertex
                if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
                    c2 = p2 - next.tangent * controlDistance * next.incomingTension;
                } else {
                    QPointF p3 = (i + 2 < n) ? tempVertices[i + 2].position : next.position;
                    double tension = (next.type == Geometry::VertexType::Smooth) ? next.incomingTension : 0.5;
                    c2 = p2 - (p3 - p1) * (tension / 3.0);
                }

                previewPath.cubicTo(c1, c2, p2);
            } else {
                previewPath.lineTo(next.position);
            }
        }

        QPen previewPen(Qt::blue, 2, Qt::DashLine);
        painter->setPen(previewPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(previewPath);
    }

    // Draw closing line preview
    if (m_isDrawing && m_vertices.size() >= 2) {
        QPen closingPen(QColor(100, 100, 255), 1, Qt::DotLine);
        painter->setPen(closingPen);

        QPointF lastPoint = m_isDragging ? m_pressPoint : m_currentPoint;
        painter->drawLine(lastPoint, m_vertices.first().position);
    }

    // Draw vertex markers
    for (const auto& vertex : m_vertices) {
        if (vertex.type == Geometry::VertexType::Sharp) {
            // Sharp vertex: red square
            painter->setBrush(Qt::red);
            painter->setPen(Qt::NoPen);
            painter->drawRect(QRectF(vertex.position - QPointF(4, 4), QSizeF(8, 8)));
        } else {
            // Smooth vertex: green circle
            painter->setBrush(Qt::green);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(vertex.position, 5, 5);
        }
    }

    // Draw segment dimensions
    QFont dimFont;
    dimFont.setPointSize(10);
    dimFont.setBold(true);
    painter->setFont(dimFont);

    for (int i = 0; i < tempVertices.size() - 1; ++i) {
        QPointF p1 = tempVertices[i].position;
        QPointF p2 = tempVertices[i + 1].position;
        QPointF delta = p2 - p1;
        double length = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (length > 1.0) {
            // Format length and angle
            QString lengthText = Units::formatLength(length, 2);
            double angle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;
            QString angleText = QString("%1°").arg(angle, 0, 'f', 1);
            QString dimensionText = QString("%1  ∠%2").arg(lengthText, angleText);

            // Position at midpoint, offset perpendicular
            QPointF midpoint = (p1 + p2) / 2.0;
            QPointF perpendicular(-delta.y(), delta.x());
            double perpLength = std::sqrt(perpendicular.x() * perpendicular.x() +
                                        perpendicular.y() * perpendicular.y());
            if (perpLength > 0.001) {
                perpendicular /= perpLength;
                perpendicular *= 12.0;
            }
            QPointF textPos = midpoint + perpendicular;

            QFontMetrics fm(dimFont);
            QRect textRect = fm.boundingRect(dimensionText);
            textRect.moveCenter(textPos.toPoint());

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 200, 230));
            painter->drawRect(textRect.adjusted(-3, -2, 3, 2));

            painter->setPen(QColor(0, 0, 180));
            painter->drawText(textRect, Qt::AlignCenter, dimensionText);
        }
    }

    // Draw current point marker (on top of everything)
    if (m_isDrawing) {
        if (m_isDragging) {
            // During drag: show smooth point with tension indicator
            QPointF delta = m_currentPoint - m_pressPoint;
            double dragDistance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
            double tension = qBound(0.1, dragDistance / 100.0, 1.0);

            // Draw tangent line from press point to current point
            painter->setPen(QPen(Qt::green, 2, Qt::SolidLine));
            painter->drawLine(m_pressPoint, m_currentPoint);

            // Draw smooth point at press point (where it will be created)
            painter->setBrush(QColor(0, 255, 0, 200));
            painter->setPen(QPen(Qt::darkGreen, 3));
            double size = 5 + tension * 8; // Size increases with tension
            painter->drawEllipse(m_pressPoint, size, size);

            // Draw current cursor position
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(Qt::green, 1));
            painter->drawEllipse(m_currentPoint, 3, 3);

            // Draw tension indicator text with background
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 255, 200));
            QString tensionText = QString("Tension: %1%").arg(qRound(tension * 100));
            QFont font = painter->font();
            font.setPointSize(11);
            font.setBold(true);
            painter->setFont(font);
            QFontMetrics fm(font);
            QRectF textRect = fm.boundingRect(tensionText);
            textRect.moveTopLeft(m_currentPoint + QPointF(15, -10));
            textRect.adjust(-4, -2, 4, 2);
            painter->drawRect(textRect);

            painter->setPen(Qt::darkGreen);
            painter->drawText(textRect, Qt::AlignCenter, tensionText);
        } else if (!m_isDragging) {
            // Simple hover: show sharp point preview
            painter->setBrush(QColor(255, 0, 0, 128));
            painter->setPen(QPen(Qt::red, 2));
            painter->drawRect(QRectF(m_currentPoint - QPointF(4, 4), QSizeF(8, 8)));
        }
    }

    painter->restore();
}

void PolylineTool::addVertex(const QPointF& point, Geometry::VertexType type, double tension, const QPointF& tangent)
{
    m_vertices.append(Geometry::PolylineVertex(point, type, tension, tangent));
    m_currentPoint = point;
    m_isDrawing = true;
    setState(ToolState::Active);

    QString typeStr = (type == Geometry::VertexType::Sharp) ? "sharp" :
                      QString("smooth (%1%)").arg(qRound(tension * 100));
    showStatusMessage(QString("Added %1 vertex (%2 points) - Double-click or Enter to finish")
                     .arg(typeStr)
                     .arg(m_vertices.size()));
}

void PolylineTool::finishPolyline()
{
    if (m_vertices.size() < 3) {
        showStatusMessage("Need at least 3 points to create a polyline");
        return;
    }

    if (!m_document) {
        return;
    }

    // Create the polyline object
    Geometry::Polyline* polyline = new Geometry::Polyline(m_vertices);
    polyline->setClosed(true);
    m_document->addObject(polyline);

    showStatusMessage(QString("Closed polyline created with %1 vertices")
                     .arg(m_vertices.size()));

    emit objectCreated();
    reset();
}

void PolylineTool::cancelPolyline()
{
    if (m_isDrawing) {
        showStatusMessage("Polyline cancelled");
    }
    reset();
}

void PolylineTool::applyLength(double lengthInMm, double angleDegrees)
{
    if (!m_isDrawing) {
        // First point - use origin or current position
        if (m_vertices.isEmpty()) {
            QPointF startPoint(0, 0);
            addVertex(startPoint, Geometry::VertexType::Sharp, 0.5, QPointF());
        }
        return;
    }

    if (m_vertices.isEmpty()) {
        return;
    }

    // Get the last vertex position
    QPointF lastPoint = m_vertices.last().position;

    QPointF delta;

    if (angleDegrees != 0.0) {
        // Use the specified angle (convert to radians)
        double angleRad = angleDegrees * M_PI / 180.0;
        delta = QPointF(std::cos(angleRad), std::sin(angleRad));
    } else {
        // Calculate direction from last point to current point
        delta = m_currentPoint - lastPoint;
        double currentLength = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (currentLength < 0.001) {
            // If no direction yet, determine from previous segment or use horizontal
            if (m_vertices.size() >= 2) {
                // Use direction of previous segment
                QPointF prevDelta = lastPoint - m_vertices[m_vertices.size() - 2].position;
                double prevLength = std::sqrt(prevDelta.x() * prevDelta.x() + prevDelta.y() * prevDelta.y());
                if (prevLength > 0.001) {
                    delta = prevDelta / prevLength;
                } else {
                    delta = QPointF(1.0, 0.0);
                }
            } else {
                // First segment, use horizontal
                delta = QPointF(1.0, 0.0);
            }
        } else {
            // Normalize direction
            delta /= currentLength;
        }
    }

    // Apply exact length
    QPointF newPoint = lastPoint + delta * lengthInMm;

    // Add the vertex with exact dimension
    addVertex(newPoint, Geometry::VertexType::Sharp, 0.5, QPointF());

    // Update current point to the new position so the preview updates
    m_currentPoint = newPoint;
}

} // namespace Tools
} // namespace PatternCAD
