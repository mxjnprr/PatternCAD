/**
 * LineTool.cpp
 *
 * Implementation of LineTool
 */

#include "LineTool.h"
#include "core/Document.h"
#include "geometry/Line.h"
#include <QPainter>

namespace PatternCAD {
namespace Tools {

LineTool::LineTool(QObject* parent)
    : Tool(parent)
    , m_hasStartPoint(false)
{
}

LineTool::~LineTool()
{
}

QString LineTool::name() const
{
    return "Line";
}

QString LineTool::description() const
{
    return "Draw line segments";
}

void LineTool::activate()
{
    Tool::activate();
    m_hasStartPoint = false;
    showStatusMessage("Click to set line start point");
}

void LineTool::reset()
{
    Tool::reset();
    m_hasStartPoint = false;
    showStatusMessage("Click to set line start point");
}

void LineTool::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPointF point = event->position().toPoint();
    point = snapToGrid(point);

    if (!m_hasStartPoint) {
        // Set start point
        startLine(point);
    } else {
        // Set end point and create line
        finishLine(point);
    }

    event->accept();
}

void LineTool::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = event->position().toPoint();
    point = snapToGrid(point);

    if (m_hasStartPoint) {
        updatePreview(point);
    }

    event->accept();
}

void LineTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reset();
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void LineTool::drawOverlay(QPainter* painter)
{
    if (m_hasStartPoint) {
        // Draw preview line
        painter->save();
        QPen pen(Qt::blue, 1, Qt::DashLine);
        painter->setPen(pen);
        painter->drawLine(m_startPoint, m_currentPoint);

        // Draw start point
        painter->setBrush(Qt::blue);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(m_startPoint, 4, 4);

        painter->restore();
    }
}

void LineTool::startLine(const QPointF& point)
{
    m_startPoint = point;
    m_currentPoint = point;
    m_hasStartPoint = true;
    setState(ToolState::Active);
    showStatusMessage(QString("Start point: (%1, %2) - Click to set end point")
                     .arg(point.x(), 0, 'f', 2)
                     .arg(point.y(), 0, 'f', 2));
}

void LineTool::updatePreview(const QPointF& point)
{
    m_currentPoint = point;

    // Calculate line length for status message
    QPointF delta = m_currentPoint - m_startPoint;
    double length = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());

    showStatusMessage(QString("End point: (%1, %2) - Length: %3")
                     .arg(point.x(), 0, 'f', 2)
                     .arg(point.y(), 0, 'f', 2)
                     .arg(length, 0, 'f', 2));
}

void LineTool::finishLine(const QPointF& point)
{
    m_currentPoint = point;
    createLine();
    reset();
}

void LineTool::createLine()
{
    if (!m_document) {
        return;
    }

    // Create new line object
    Geometry::Line* line = new Geometry::Line(m_startPoint, m_currentPoint);
    m_document->addObject(line);

    showStatusMessage("Line created");
}

} // namespace Tools
} // namespace PatternCAD
