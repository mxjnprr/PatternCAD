/**
 * CircleTool.cpp
 *
 * Implementation of CircleTool
 */

#include "CircleTool.h"
#include "core/Document.h"
#include "core/Units.h"
#include "geometry/Circle.h"
#include <QMouseEvent>
#include <QPainter>
#include <QFontMetrics>
#include <cmath>

namespace PatternCAD {
namespace Tools {

CircleTool::CircleTool(QObject* parent)
    : Tool(parent)
    , m_centerSet(false)
{
}

CircleTool::~CircleTool()
{
}

QString CircleTool::name() const
{
    return "Circle";
}

QString CircleTool::description() const
{
    return "Click center, then radius point";
}

void CircleTool::activate()
{
    Tool::activate();
    m_centerSet = false;
    showStatusMessage("Click to set circle center");
}

void CircleTool::reset()
{
    Tool::reset();
    m_centerSet = false;
    m_centerPoint = QPointF();
    m_radiusPoint = QPointF();
    showStatusMessage("Click to set circle center");
}

void CircleTool::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPointF point = mapToScene(event->pos());
    point = snapToGrid(point);

    if (!m_centerSet) {
        // Set center point
        startCircle(point);
    } else {
        // Set radius point and create circle
        finishCircle(point);
    }

    event->accept();
}

void CircleTool::mouseMoveEvent(QMouseEvent* event)
{
    if (m_centerSet) {
        QPointF point = mapToScene(event->pos());
        point = snapToGrid(point);
        updatePreview(point);
    }

    event->accept();
}

void CircleTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reset();
        event->accept();
    } else {
        Tool::keyPressEvent(event);
    }
}

void CircleTool::drawOverlay(QPainter* painter)
{
    if (m_centerSet) {
        // Calculate radius
        QPointF delta = m_radiusPoint - m_centerPoint;
        double radius = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        painter->save();

        // Draw preview circle
        QPen pen(Qt::blue, 1, Qt::DashLine);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(m_centerPoint, radius, radius);

        // Draw center marker
        painter->setPen(QPen(Qt::red, 2));
        painter->drawLine(m_centerPoint + QPointF(-6, 0), m_centerPoint + QPointF(6, 0));
        painter->drawLine(m_centerPoint + QPointF(0, -6), m_centerPoint + QPointF(0, 6));

        if (radius > 1.0) {
            // Draw radius line
            painter->setPen(QPen(Qt::red, 1, Qt::DotLine));
            painter->drawLine(m_centerPoint, m_radiusPoint);

            // Format dimensions with units
            QString radiusText = "R " + Units::formatLength(radius, 2);
            QString diameterText = "Ø " + Units::formatLength(radius * 2, 2);

            QFont font = painter->font();
            font.setPointSize(10);
            font.setBold(true);
            painter->setFont(font);

            QFontMetrics metrics(font);

            // Draw radius text at midpoint of radius line
            QPointF radiusTextPos = (m_centerPoint + m_radiusPoint) / 2.0;

            // Offset perpendicular to radius line
            QPointF perpendicular(-delta.y(), delta.x());
            double perpLength = std::sqrt(perpendicular.x() * perpendicular.x() +
                                        perpendicular.y() * perpendicular.y());
            if (perpLength > 0.001) {
                perpendicular /= perpLength;
                perpendicular *= 10.0;
            }
            radiusTextPos += perpendicular;

            QRect radiusTextRect = metrics.boundingRect(radiusText);
            radiusTextRect.moveCenter(radiusTextPos.toPoint());

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 200, 230));
            painter->drawRect(radiusTextRect.adjusted(-3, -2, 3, 2));

            painter->setPen(QColor(0, 0, 180));
            painter->drawText(radiusTextRect, Qt::AlignCenter, radiusText);

            // Draw diameter text at top of circle
            QPointF diameterTextPos = m_centerPoint + QPointF(0, -radius - 15);

            QRect diameterTextRect = metrics.boundingRect(diameterText);
            diameterTextRect.moveCenter(diameterTextPos.toPoint());

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 255, 200, 230));
            painter->drawRect(diameterTextRect.adjusted(-3, -2, 3, 2));

            painter->setPen(QColor(0, 0, 180));
            painter->drawText(diameterTextRect, Qt::AlignCenter, diameterText);
        }

        painter->restore();
    }
}

void CircleTool::startCircle(const QPointF& point)
{
    m_centerPoint = point;
    m_radiusPoint = point;
    m_centerSet = true;
    setState(ToolState::Active);
    showStatusMessage(QString("Center: (%1, %2) - Click to set radius")
                     .arg(point.x(), 0, 'f', 2)
                     .arg(point.y(), 0, 'f', 2));
}

void CircleTool::updatePreview(const QPointF& point)
{
    m_radiusPoint = point;

    // Calculate radius for status message
    QPointF delta = m_radiusPoint - m_centerPoint;
    double radius = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

    showStatusMessage(QString("Radius: %1")
                     .arg(radius, 0, 'f', 2));
}

void CircleTool::finishCircle(const QPointF& point)
{
    m_radiusPoint = point;
    createCircle();
    reset();
}

void CircleTool::createCircle()
{
    if (!m_document) {
        return;
    }

    // Calculate radius
    QPointF delta = m_radiusPoint - m_centerPoint;
    double radius = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

    // Only create circle if radius is meaningful
    if (radius > 0.1) {
        Geometry::Circle* circle = new Geometry::Circle(m_centerPoint, radius);
        m_document->addObject(circle);

        showStatusMessage(QString("Circle created (radius: %1)")
                         .arg(radius, 0, 'f', 1));
        emit objectCreated();
    } else {
        showStatusMessage("Circle too small - cancelled");
    }
}

} // namespace Tools
} // namespace PatternCAD
