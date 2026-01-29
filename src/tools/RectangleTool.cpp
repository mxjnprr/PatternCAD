/**
 * RectangleTool.cpp
 *
 * Implementation of RectangleTool
 */

#include "RectangleTool.h"
#include "core/Document.h"
#include "core/Units.h"
#include "geometry/Rectangle.h"
#include <QMouseEvent>
#include <QPainter>
#include <QFontMetrics>
#include <cmath>

namespace PatternCAD {
namespace Tools {

RectangleTool::RectangleTool(QObject* parent)
    : Tool(parent)
    , m_firstPointSet(false)
{
}

RectangleTool::~RectangleTool()
{
}

QString RectangleTool::name() const
{
    return "Rectangle";
}

QString RectangleTool::description() const
{
    return "Click two opposite corners";
}

void RectangleTool::activate()
{
    Tool::activate();
    m_firstPointSet = false;
    showStatusMessage("Click to set first corner");
}

void RectangleTool::reset()
{
    Tool::reset();
    m_firstPointSet = false;
    m_startPoint = QPointF();
    m_endPoint = QPointF();
    showStatusMessage("Click to set first corner");
}

void RectangleTool::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPointF point = mapToScene(event->pos());
    point = snapToGrid(point);

    if (!m_firstPointSet) {
        // Set first corner
        startRectangle(point);
    } else {
        // Set second corner and create rectangle
        finishRectangle(point);
    }

    event->accept();
}

void RectangleTool::mouseMoveEvent(QMouseEvent* event)
{
    if (m_firstPointSet) {
        QPointF point = mapToScene(event->pos());
        point = snapToGrid(point);
        updatePreview(point);
    }

    event->accept();
}

void RectangleTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reset();
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void RectangleTool::drawOverlay(QPainter* painter)
{
    if (m_firstPointSet) {
        painter->save();

        // Draw preview rectangle
        QPen pen(Qt::blue, 1, Qt::DashLine);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        QRectF rect(m_startPoint, m_endPoint);
        QRectF normalizedRect = rect.normalized();
        painter->drawRect(normalizedRect);

        // Calculate dimensions
        double width = std::abs(m_endPoint.x() - m_startPoint.x());
        double height = std::abs(m_endPoint.y() - m_startPoint.y());

        if (width > 1.0 && height > 1.0) {
            // Format dimensions with units
            QString widthText = Units::formatLength(width, 2);
            QString heightText = Units::formatLength(height, 2);

            QFont font = painter->font();
            font.setPointSize(10);
            font.setBold(true);
            painter->setFont(font);

            QFontMetrics metrics(font);

            // Draw width dimension at bottom
            QPointF bottomLeft = normalizedRect.bottomLeft();
            QPointF bottomRight = normalizedRect.bottomRight();
            QPointF widthTextPos = (bottomLeft + bottomRight) / 2.0 + QPointF(0, 20);

            QRect widthTextRect = metrics.boundingRect(widthText);
            widthTextRect.moveCenter(widthTextPos.toPoint());

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 200, 230));
            painter->drawRect(widthTextRect.adjusted(-3, -2, 3, 2));

            painter->setPen(QColor(0, 0, 180));
            painter->drawText(widthTextRect, Qt::AlignCenter, widthText);

            // Draw height dimension at right
            QPointF topRight = normalizedRect.topRight();
            bottomRight = normalizedRect.bottomRight();
            QPointF heightTextPos = (topRight + bottomRight) / 2.0 + QPointF(25, 0);

            QRect heightTextRect = metrics.boundingRect(heightText);
            heightTextRect.moveCenter(heightTextPos.toPoint());

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 200, 230));
            painter->drawRect(heightTextRect.adjusted(-3, -2, 3, 2));

            painter->setPen(QColor(0, 0, 180));
            painter->drawText(heightTextRect, Qt::AlignCenter, heightText);

            // Draw dimension lines
            QPen dimPen(QColor(180, 180, 180), 1, Qt::DotLine);
            painter->setPen(dimPen);

            // Width dimension line
            painter->drawLine(bottomLeft + QPointF(0, 15), bottomRight + QPointF(0, 15));

            // Height dimension line
            painter->drawLine(topRight + QPointF(20, 0), bottomRight + QPointF(20, 0));
        }

        painter->restore();
    }
}

void RectangleTool::startRectangle(const QPointF& point)
{
    m_startPoint = point;
    m_endPoint = point;
    m_firstPointSet = true;
    setState(ToolState::Active);
    showStatusMessage(QString("First corner: (%1, %2) - Click opposite corner")
                     .arg(point.x(), 0, 'f', 2)
                     .arg(point.y(), 0, 'f', 2));
}

void RectangleTool::updatePreview(const QPointF& point)
{
    m_endPoint = point;

    // Calculate dimensions for status message
    double width = std::abs(m_endPoint.x() - m_startPoint.x());
    double height = std::abs(m_endPoint.y() - m_startPoint.y());

    showStatusMessage(QString("Size: %1 x %2")
                     .arg(width, 0, 'f', 2)
                     .arg(height, 0, 'f', 2));
}

void RectangleTool::finishRectangle(const QPointF& point)
{
    m_endPoint = point;
    createRectangle();
    reset();
}

void RectangleTool::createRectangle()
{
    if (!m_document) {
        return;
    }

    // Calculate width and height
    double width = m_endPoint.x() - m_startPoint.x();
    double height = m_endPoint.y() - m_startPoint.y();

    // Only create rectangle if dimensions are meaningful
    if (std::abs(width) > 1.0 && std::abs(height) > 1.0) {
        Geometry::Rectangle* rect = new Geometry::Rectangle(m_startPoint, width, height);
        m_document->addObject(rect);

        showStatusMessage(QString("Rectangle created (%1 x %2)")
                         .arg(std::abs(width), 0, 'f', 1)
                         .arg(std::abs(height), 0, 'f', 1));
        emit objectCreated();
    } else {
        showStatusMessage("Rectangle too small - cancelled");
    }
}

} // namespace Tools
} // namespace PatternCAD
