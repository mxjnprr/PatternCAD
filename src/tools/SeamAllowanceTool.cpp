/**
 * SeamAllowanceTool.cpp
 *
 * Implementation of SeamAllowanceTool
 */

#include "SeamAllowanceTool.h"
#include "geometry/GeometryObject.h"
#include "geometry/Polyline.h"
#include "core/Document.h"
#include "ui/Canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QInputDialog>
#include <cmath>

namespace PatternCAD {
namespace Tools {

SeamAllowanceTool::SeamAllowanceTool(QObject* parent)
    : Tool(parent)
    , m_mode(Mode::SelectingPolyline)
    , m_width(10.0)  // Default 10mm
    , m_cornerType(CornerType::Miter)  // Miter for sharp corners
    , m_selectedPolyline(nullptr)
    , m_hoveredVertexIndex(-1)
    , m_startVertexIndex(-1)
    , m_lastClickedVertex(-1)
{
    m_doubleClickTimer.invalidate();
}

SeamAllowanceTool::~SeamAllowanceTool()
{
}

QString SeamAllowanceTool::name() const
{
    return "SeamAllowance";
}

QString SeamAllowanceTool::description() const
{
    return "Apply seam allowance to pattern pieces (S)";
}

QCursor SeamAllowanceTool::cursor() const
{
    return Qt::CrossCursor;
}

void SeamAllowanceTool::activate()
{
    Tool::activate();
    m_mode = Mode::SelectingPolyline;
    m_selectedPolyline = nullptr;
    m_hoveredVertexIndex = -1;
    m_startVertexIndex = -1;
    m_lastClickedVertex = -1;
    m_doubleClickTimer.invalidate();
    updateStatusMessage();
}

void SeamAllowanceTool::reset()
{
    m_mode = Mode::SelectingPolyline;
    m_selectedPolyline = nullptr;
    m_hoveredVertexIndex = -1;
    m_startVertexIndex = -1;
    m_lastClickedVertex = -1;
    m_doubleClickTimer.invalidate();
    updateStatusMessage();
}

void SeamAllowanceTool::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        if (m_mode == Mode::SelectingPolyline) {
            // Select a polyline
            Geometry::Polyline* polyline = findPolylineAt(scenePos);
            if (polyline) {
                selectPolyline(polyline);
            }
        } else if (m_mode == Mode::SelectingStartPoint) {
            int vertexIndex = findVertexAt(m_selectedPolyline, scenePos);
            if (vertexIndex >= 0) {
                // Check for double-click (same vertex within time limit)
                if (m_lastClickedVertex == vertexIndex && 
                    m_doubleClickTimer.isValid() && 
                    m_doubleClickTimer.elapsed() < DOUBLE_CLICK_MS) {
                    // Double-click on same vertex = full contour
                    applyFullContourSeamAllowance();
                    reset();
                } else {
                    // First click - select start vertex
                    m_startVertexIndex = vertexIndex;
                    m_mode = Mode::SelectingEndPoint;
                    m_lastClickedVertex = vertexIndex;
                    m_doubleClickTimer.restart();
                    updateStatusMessage();
                }
                if (m_canvas) m_canvas->update();
            }
        } else if (m_mode == Mode::SelectingEndPoint) {
            int vertexIndex = findVertexAt(m_selectedPolyline, scenePos);
            if (vertexIndex >= 0) {
                if (vertexIndex == m_startVertexIndex) {
                    // Same vertex = full contour
                    applyFullContourSeamAllowance();
                } else {
                    // Different vertex = range
                    applySeamAllowance(m_startVertexIndex, vertexIndex);
                }
                reset();
                if (m_canvas) m_canvas->update();
            }
        }
    } else if (event->button() == Qt::RightButton) {
        // Right-click to cancel and go back
        reset();
        if (m_canvas) m_canvas->update();
    }
}

void SeamAllowanceTool::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_document) return;

    QPointF scenePos = mapToScene(event->pos());

    if (m_mode == Mode::SelectingPolyline) {
        // Hover detection for polylines
        if (m_canvas) m_canvas->update();
    } else if (m_mode == Mode::SelectingStartPoint || m_mode == Mode::SelectingEndPoint) {
        // Hover detection for vertices
        if (m_selectedPolyline) {
            int oldHoveredVertex = m_hoveredVertexIndex;
            m_hoveredVertexIndex = findVertexAt(m_selectedPolyline, scenePos);

            if (oldHoveredVertex != m_hoveredVertexIndex) {
                updateStatusMessage();
                if (m_canvas) m_canvas->update();
            }
        }
    }
}

void SeamAllowanceTool::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void SeamAllowanceTool::mouseDoubleClickEvent(QMouseEvent* event)
{
    // Double-click is handled in mousePressEvent via timer
    Q_UNUSED(event);
}

void SeamAllowanceTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reset();
        if (m_canvas) m_canvas->update();
    }
}

void SeamAllowanceTool::drawOverlay(QPainter* painter)
{
    if (!painter || !m_document) return;

    painter->save();

    if (m_selectedPolyline) {
        drawVertexHighlights(painter);
        
        if (m_mode == Mode::SelectingEndPoint && m_startVertexIndex >= 0) {
            drawRangePreview(painter);
        }
    }

    painter->restore();
}

void SeamAllowanceTool::setWidth(double width)
{
    if (m_width != width && width >= 0.0) {
        m_width = width;
        emit widthChanged(m_width);
    }
}

void SeamAllowanceTool::setCornerType(CornerType type)
{
    if (m_cornerType != type) {
        m_cornerType = type;
        emit cornerTypeChanged(m_cornerType);
    }
}

Geometry::Polyline* SeamAllowanceTool::findPolylineAt(const QPointF& point) const
{
    if (!m_document) return nullptr;

    const auto& objects = m_document->objects();

    // Search in reverse order (top to bottom)
    for (int i = objects.size() - 1; i >= 0; --i) {
        Geometry::GeometryObject* obj = objects[i];

        // Only consider polylines
        if (auto* polyline = qobject_cast<Geometry::Polyline*>(obj)) {
            if (polyline->contains(point)) {
                return polyline;
            }
        }
    }

    return nullptr;
}

int SeamAllowanceTool::findVertexAt(Geometry::Polyline* polyline, const QPointF& point) const
{
    if (!polyline) return -1;

    const double tolerance = 8.0;  // 8 pixels tolerance for vertex selection
    const QVector<Geometry::PolylineVertex>& vertices = polyline->vertices();

    for (int i = 0; i < vertices.size(); ++i) {
        QPointF delta = point - vertices[i].position;
        double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (distance <= tolerance) {
            return i;
        }
    }

    return -1;
}

void SeamAllowanceTool::selectPolyline(Geometry::Polyline* polyline)
{
    if (!polyline) return;

    m_selectedPolyline = polyline;
    m_mode = Mode::SelectingStartPoint;
    m_startVertexIndex = -1;
    m_hoveredVertexIndex = -1;

    updateStatusMessage();
    if (m_canvas) m_canvas->update();
}

void SeamAllowanceTool::applySeamAllowance(int startVertex, int endVertex)
{
    if (!m_selectedPolyline) return;

    SeamAllowance* seamAllowance = m_selectedPolyline->seamAllowance();
    if (!seamAllowance) return;

    // Get parent widget for dialog
    QWidget* parentWidget = m_canvas ? m_canvas->parentWidget() : nullptr;

    // Show dialog to ask for width
    bool ok;
    double width = QInputDialog::getDouble(
        parentWidget,
        tr("Seam Allowance"),
        tr("Width (mm):"),
        m_width,  // Default value
        0.0,       // Min (0 = remove seam)
        100.0,     // Max
        1,         // Decimals
        &ok
    );

    if (!ok) {
        // User cancelled
        showStatusMessage("Seam allowance cancelled");
        return;
    }

    m_width = width;

    // ADD a range with the specified width (width=0 creates a "no seam" section)
    seamAllowance->setCornerType(m_cornerType);
    seamAllowance->addRange(startVertex, endVertex, width);
    
    if (width <= 0.0) {
        showStatusMessage(QString("No seam allowance from vertex %1 to %2")
            .arg(startVertex + 1)
            .arg(endVertex + 1));
    } else {
        showStatusMessage(QString("Seam allowance added from vertex %1 to %2 (%.1f mm)")
            .arg(startVertex + 1)
            .arg(endVertex + 1)
            .arg(width));
    }

    if (m_document) {
        m_document->setModified(true);
    }
}

void SeamAllowanceTool::applyFullContourSeamAllowance()
{
    if (!m_selectedPolyline) return;

    SeamAllowance* seamAllowance = m_selectedPolyline->seamAllowance();
    if (!seamAllowance) return;

    // Get parent widget for dialog
    QWidget* parentWidget = m_canvas ? m_canvas->parentWidget() : nullptr;

    // Show dialog to ask for width
    bool ok;
    double width = QInputDialog::getDouble(
        parentWidget,
        tr("Seam Allowance"),
        tr("Width (mm):"),
        m_width,  // Default value
        0.0,       // Min (0 = remove seam)
        100.0,     // Max
        1,         // Decimals
        &ok
    );

    if (!ok) {
        // User cancelled
        showStatusMessage("Seam allowance cancelled");
        return;
    }

    m_width = width;

    if (width <= 0.0) {
        // Width 0 = clear all seam allowances
        seamAllowance->clearRanges();
        showStatusMessage("All seam allowances removed");
    } else {
        // ADD a full contour range
        seamAllowance->setCornerType(m_cornerType);
        seamAllowance->addFullContour(width);
        showStatusMessage(QString("Seam allowance added to full contour (%.1f mm)").arg(width));
    }

    if (m_document) {
        m_document->setModified(true);
    }
}

void SeamAllowanceTool::drawVertexHighlights(QPainter* painter) const
{
    if (!m_selectedPolyline) return;

    const QVector<Geometry::PolylineVertex>& vertices = m_selectedPolyline->vertices();
    const double vertexRadius = 6.0;

    for (int i = 0; i < vertices.size(); ++i) {
        QPointF pos = vertices[i].position;
        QColor fillColor;
        QColor strokeColor = Qt::black;
        int strokeWidth = 1;

        if (i == m_startVertexIndex) {
            // Selected start vertex - green
            fillColor = QColor(0, 200, 0, 200);
            strokeWidth = 2;
        } else if (i == m_hoveredVertexIndex) {
            // Hovered vertex - yellow
            fillColor = QColor(255, 255, 0, 200);
            strokeWidth = 2;
        } else {
            // Normal vertex - white
            fillColor = QColor(255, 255, 255, 180);
        }

        painter->setPen(QPen(strokeColor, strokeWidth));
        painter->setBrush(fillColor);
        painter->drawEllipse(pos, vertexRadius, vertexRadius);

        // Draw vertex number
        painter->setPen(Qt::black);
        painter->drawText(pos + QPointF(8, -8), QString::number(i + 1));
    }
}

void SeamAllowanceTool::drawRangePreview(QPainter* painter) const
{
    if (!m_selectedPolyline || m_startVertexIndex < 0 || m_hoveredVertexIndex < 0) return;
    if (m_hoveredVertexIndex == m_startVertexIndex) return;  // Don't preview same vertex

    const QVector<Geometry::PolylineVertex>& vertices = m_selectedPolyline->vertices();
    int n = vertices.size();

    // Draw edges from start to hovered (clockwise) in green
    QPen previewPen(QColor(0, 200, 0, 180), 4, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(previewPen);

    int current = m_startVertexIndex;
    while (current != m_hoveredVertexIndex) {
        int nextIdx = (current + 1) % n;
        painter->drawLine(vertices[current].position, vertices[nextIdx].position);
        current = nextIdx;
    }
}

void SeamAllowanceTool::updateStatusMessage()
{
    QString message;

    if (m_mode == Mode::SelectingPolyline) {
        message = QString("Click on a pattern piece to apply seam allowance (%.1f mm)").arg(m_width);
    } else if (m_mode == Mode::SelectingStartPoint) {
        message = "Click on a vertex to start seam allowance range | Double-click for full contour | Esc to cancel";
    } else if (m_mode == Mode::SelectingEndPoint) {
        if (m_hoveredVertexIndex >= 0 && m_hoveredVertexIndex != m_startVertexIndex) {
            message = QString("Click vertex %1 to apply seam | Same vertex for full contour | Esc to cancel")
                .arg(m_hoveredVertexIndex + 1);
        } else {
            message = QString("Click another vertex to define range end (from vertex %1) | Esc to cancel")
                .arg(m_startVertexIndex + 1);
        }
    }

    showStatusMessage(message);
}

} // namespace Tools
} // namespace PatternCAD
