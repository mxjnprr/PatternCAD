/**
 * PointTool.cpp
 *
 * Implementation of PointTool
 */

#include "PointTool.h"
#include "core/Document.h"
#include "geometry/Point2D.h"
#include <QMouseEvent>

namespace PatternCAD {
namespace Tools {

PointTool::PointTool(QObject* parent)
    : Tool(parent)
{
}

PointTool::~PointTool()
{
}

QString PointTool::name() const
{
    return "Point";
}

QString PointTool::description() const
{
    return "Click to place a point";
}

void PointTool::activate()
{
    Tool::activate();
    showStatusMessage("Click to place a point");
}

void PointTool::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPointF point = mapToScene(event->pos());
    point = snapToGrid(point);

    if (m_document) {
        // Create new point object
        Geometry::Point2D* pointObj = new Geometry::Point2D(point);
        m_document->addObject(pointObj);

        showStatusMessage(QString("Point created at (%1, %2)")
                         .arg(point.x(), 0, 'f', 1)
                         .arg(point.y(), 0, 'f', 1));

        emit objectCreated();
    }

    event->accept();
}

} // namespace Tools
} // namespace PatternCAD
