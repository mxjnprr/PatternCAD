/**
 * LineTool.cpp
 *
 * Implementation of LineTool
 */

#include "LineTool.h"
#include "core/Document.h"
#include "core/Units.h"
#include "geometry/Line.h"
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <cmath>

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

    QPointF point = mapToScene(event->pos());
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
    QPointF point = mapToScene(event->pos());
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
    } else if (event->key() == Qt::Key_Tab && m_hasStartPoint) {
        // Calculate current length and angle
        QPointF delta = m_currentPoint - m_startPoint;
        double currentLength = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        double currentAngle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;

        // Request dimension input for line length
        qDebug() << "LineTool: Tab pressed, emitting dimensionInputRequested";
        emit dimensionInputRequested("length", currentLength, currentAngle);
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void LineTool::drawOverlay(QPainter* painter)
{
    if (m_hasStartPoint) {
        painter->save();

        // Draw preview line
        QPen pen(Qt::blue, 1, Qt::DashLine);
        painter->setPen(pen);
        painter->drawLine(m_startPoint, m_currentPoint);

        // Draw start point
        painter->setBrush(Qt::blue);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(m_startPoint, 4, 4);

        // Calculate line length and angle
        QPointF delta = m_currentPoint - m_startPoint;
        double length = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        double angle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;

        // Draw dimension annotation
        if (length > 1.0) {
            // Format length with current units
            QString lengthText = Units::formatLength(length, 2);
            QString angleText = QString("%1°").arg(angle, 0, 'f', 1);
            QString dimensionText = QString("%1  ∠%2").arg(lengthText, angleText);

            // Position text at midpoint of line
            QPointF midpoint = (m_startPoint + m_currentPoint) / 2.0;

            // Offset perpendicular to line
            QPointF perpendicular(-delta.y(), delta.x());
            double perpLength = std::sqrt(perpendicular.x() * perpendicular.x() +
                                        perpendicular.y() * perpendicular.y());
            if (perpLength > 0.001) {
                perpendicular /= perpLength;
                perpendicular *= 15.0; // Offset distance
            }

            QPointF textPos = midpoint + perpendicular;

            // Draw text background
            QFont font = painter->font();
            font.setPointSize(10);
            font.setBold(true);
            painter->setFont(font);

            QFontMetrics metrics(font);
            QRect textRect = metrics.boundingRect(dimensionText);
            textRect.moveCenter(textPos.toPoint());

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 200, 230)); // Light yellow background
            painter->drawRect(textRect.adjusted(-3, -2, 3, 2));

            // Draw text
            painter->setPen(QColor(0, 0, 180)); // Dark blue text
            painter->drawText(textRect, Qt::AlignCenter, dimensionText);
        }

        // Draw coordinates at current point
        QString coordText = QString("(%1, %2)")
            .arg(Units::formatLength(m_currentPoint.x(), 1))
            .arg(Units::formatLength(m_currentPoint.y(), 1));

        QFont coordFont = painter->font();
        coordFont.setPointSize(9);
        painter->setFont(coordFont);

        QFontMetrics coordMetrics(coordFont);
        QRect coordRect = coordMetrics.boundingRect(coordText);
        coordRect.moveBottomLeft((m_currentPoint + QPointF(8, -8)).toPoint());

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(255, 255, 255, 200));
        painter->drawRect(coordRect.adjusted(-2, -1, 2, 1));

        painter->setPen(Qt::darkGray);
        painter->drawText(coordRect, Qt::AlignCenter, coordText);

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
    emit objectCreated();
}

void LineTool::applyLength(double lengthInMm)
{
    if (!m_hasStartPoint) {
        return;
    }

    // Calculate direction from start to current point
    QPointF delta = m_currentPoint - m_startPoint;
    double currentLength = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

    if (currentLength < 0.001) {
        // If no direction yet, use horizontal
        delta = QPointF(1.0, 0.0);
    } else {
        // Normalize direction
        delta /= currentLength;
    }

    // Apply exact length
    m_currentPoint = m_startPoint + delta * lengthInMm;

    // Create the line with exact dimension
    finishLine(m_currentPoint);
}

} // namespace Tools
} // namespace PatternCAD
