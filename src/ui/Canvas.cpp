/**
 * Canvas.cpp
 *
 * Implementation of Canvas viewport
 */

#include "Canvas.h"
#include "DimensionRenderer.h"
#include "core/Document.h"
#include "core/Units.h"
#include "tools/Tool.h"
#include "geometry/GeometryObject.h"
#include <QPainter>
#include <QScrollBar>
#include <cmath>

namespace PatternCAD {
namespace UI {

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent)
    , m_scene(nullptr)
    , m_document(nullptr)
    , m_activeTool(nullptr)
    , m_dimensionRenderer(new DimensionRenderer())
    , m_zoomLevel(1.0)
    , m_gridVisible(true)
    , m_snapToGrid(true)
{
    setupScene();

    // Configure view
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::NoDrag);

    // TODO: Configure view settings
    // TODO: Connect signals
}

Canvas::~Canvas()
{
    // Cleanup
    if (m_scene) {
        delete m_scene;
    }
    if (m_dimensionRenderer) {
        delete m_dimensionRenderer;
    }
}

void Canvas::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    setScene(m_scene);

    // TODO: Initialize scene items
}

Document* Canvas::document() const
{
    return m_document;
}

void Canvas::setDocument(Document* document)
{
    m_document = document;

    if (m_document) {
        // Connect document signals to trigger redraws
        connect(m_document, &Document::objectAdded, this, [this]() {
            viewport()->update();
        });
        connect(m_document, &Document::objectRemoved, this, [this]() {
            viewport()->update();
        });
        connect(m_document, &Document::objectChanged, this, [this]() {
            viewport()->update();
        });
        connect(m_document, &Document::layerVisibilityChanged, this, [this]() {
            viewport()->update();
        });
    }
}

void Canvas::zoomIn()
{
    setZoomLevel(m_zoomLevel * 1.2);
}

void Canvas::zoomOut()
{
    setZoomLevel(m_zoomLevel / 1.2);
}

void Canvas::zoomFit()
{
    // TODO: Fit view to content bounds
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void Canvas::zoomReset()
{
    setZoomLevel(1.0);
}

void Canvas::setZoomLevel(double level)
{
    // Clamp zoom level
    level = qBound(0.1, level, 10.0);

    if (m_zoomLevel != level) {
        double scaleFactor = level / m_zoomLevel;
        scale(scaleFactor, scaleFactor);
        m_zoomLevel = level;
        emit zoomChanged(m_zoomLevel);
    }
}

double Canvas::zoomLevel() const
{
    return m_zoomLevel;
}

bool Canvas::gridVisible() const
{
    return m_gridVisible;
}

void Canvas::setGridVisible(bool visible)
{
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        viewport()->update();
    }
}

bool Canvas::snapToGrid() const
{
    return m_snapToGrid;
}

void Canvas::setSnapToGrid(bool snap)
{
    m_snapToGrid = snap;
}

QPointF Canvas::snapPoint(const QPointF& point) const
{
    if (!m_snapToGrid) {
        return point;
    }

    // TODO: Get grid spacing from document/project
    double gridSpacing = 10.0;

    double x = std::round(point.x() / gridSpacing) * gridSpacing;
    double y = std::round(point.y() / gridSpacing) * gridSpacing;

    return QPointF(x, y);
}

void Canvas::setActiveTool(Tools::Tool* tool)
{
    m_activeTool = tool;
    if (tool) {
        tool->setCanvas(this);
        tool->setDocument(m_document);
        setCursor(tool->cursor());
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

Tools::Tool* Canvas::activeTool() const
{
    return m_activeTool;
}

bool Canvas::event(QEvent* event)
{
    // Intercept Tab key before Qt's focus system consumes it
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            keyPressEvent(keyEvent);
            if (keyEvent->isAccepted()) {
                return true;
            }
        }
    }
    return QGraphicsView::event(event);
}

void Canvas::wheelEvent(QWheelEvent* event)
{
    // Zoom with mouse wheel
    if (event->modifiers() & Qt::ControlModifier) {
        double delta = event->angleDelta().y() / 120.0;
        double factor = std::pow(1.2, delta);
        setZoomLevel(m_zoomLevel * factor);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos = mapToScene(event->pos());

    if (m_activeTool) {
        m_activeTool->mousePressEvent(event);
    }

    QGraphicsView::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());
    m_lastMousePos = scenePos;

    emit cursorPositionChanged(scenePos);

    if (m_activeTool) {
        m_activeTool->mouseMoveEvent(event);
        viewport()->update(); // Update to show tool preview
    }

    QGraphicsView::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_activeTool) {
        m_activeTool->mouseReleaseEvent(event);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
    if (m_activeTool) {
        m_activeTool->keyPressEvent(event);

        // Special handling for Escape: after tool processes it, return to Select tool
        if (event->key() == Qt::Key_Escape && m_activeTool->name() != "Select") {
            // Signal MainWindow to switch to Select tool
            emit escapePressed();
            event->accept();
            return;
        }

        if (event->isAccepted()) {
            return; // Don't pass to base class if tool handled it
        }
    }

    QGraphicsView::keyPressEvent(event);
}

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    // Draw white background
    painter->fillRect(rect, Qt::white);

    // Draw grid if visible
    if (m_gridVisible) {
        drawGrid(painter, rect);
    }
}

void Canvas::drawForeground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    if (!m_document) {
        return;
    }

    // Render all document objects that are on visible layers
    painter->setRenderHint(QPainter::Antialiasing);
    for (Geometry::GeometryObject* obj : m_document->objects()) {
        if (obj && m_document->isLayerVisible(obj->layer())) {
            QColor layerColor = m_document->layerColor(obj->layer());
            obj->draw(painter, layerColor);
        }
    }

    // Render dimensions for selected objects
    if (m_dimensionRenderer) {
        for (Geometry::GeometryObject* obj : m_document->objects()) {
            if (obj && m_document->isLayerVisible(obj->layer())) {
                m_dimensionRenderer->renderDimensions(painter, obj);
            }
        }
    }

    // Let active tool draw preview/overlay
    if (m_activeTool) {
        m_activeTool->drawOverlay(painter);
    }
}

void Canvas::updateGrid()
{
    viewport()->update();
}

void Canvas::drawGrid(QPainter* painter, const QRectF& rect)
{
    // TODO: Get grid spacing from document/project
    double gridSpacing = 10.0;

    // Calculate grid bounds
    int left = static_cast<int>(std::floor(rect.left() / gridSpacing));
    int right = static_cast<int>(std::ceil(rect.right() / gridSpacing));
    int top = static_cast<int>(std::floor(rect.top() / gridSpacing));
    int bottom = static_cast<int>(std::ceil(rect.bottom() / gridSpacing));

    // Draw grid lines
    QPen gridPen(QColor(220, 220, 220));
    painter->setPen(gridPen);

    for (int i = left; i <= right; ++i) {
        double x = i * gridSpacing;
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    for (int i = top; i <= bottom; ++i) {
        double y = i * gridSpacing;
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }

    // Draw axes
    QPen axisPen(QColor(180, 180, 180), 2);
    painter->setPen(axisPen);
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));
}

} // namespace UI
} // namespace PatternCAD
