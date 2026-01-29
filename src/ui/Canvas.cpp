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
#include <QMenu>
#include <QContextMenuEvent>
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
    , m_isPanning(false)
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

void Canvas::zoomToSelection()
{
    if (!m_document) return;

    auto selected = m_document->selectedObjects();
    if (selected.isEmpty()) return;

    // Calculate bounding rect of all selected objects
    QRectF bounds;
    for (auto* obj : selected) {
        QRectF objBounds = obj->boundingRect();
        if (bounds.isNull()) {
            bounds = objBounds;
        } else {
            bounds = bounds.united(objBounds);
        }
    }

    if (!bounds.isNull()) {
        // Add some margin (10%)
        bounds = bounds.adjusted(-bounds.width() * 0.1, -bounds.height() * 0.1,
                                 bounds.width() * 0.1, bounds.height() * 0.1);
        fitInView(bounds, Qt::KeepAspectRatio);
        // Update zoom level based on transform
        m_zoomLevel = transform().m11();
        emit zoomChanged(m_zoomLevel);
    }
}

void Canvas::zoomToActual()
{
    setZoomLevel(1.0);
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
    // Zoom with mouse wheel (no modifier needed)
    // Use factor 1.1 for smoother, more controlled zoom
    double delta = event->angleDelta().y() / 120.0;
    double factor = std::pow(1.1, delta);
    setZoomLevel(m_zoomLevel * factor);
    event->accept();
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos = mapToScene(event->pos());

    // Middle mouse button or H key + left button for panning
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_panStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

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

    // Handle panning
    if (m_isPanning) {
        QPoint delta = event->pos() - m_panStartPos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_panStartPos = event->pos();
        event->accept();
        return;
    }

    if (m_activeTool) {
        m_activeTool->mouseMoveEvent(event);
        viewport()->update(); // Update to show tool preview
    }

    QGraphicsView::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    // End panning
    if (event->button() == Qt::MiddleButton && m_isPanning) {
        m_isPanning = false;
        if (m_activeTool) {
            setCursor(m_activeTool->cursor());
        } else {
            setCursor(Qt::ArrowCursor);
        }
        event->accept();
        return;
    }

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

void Canvas::contextMenuEvent(QContextMenuEvent* event)
{
    // Context menu is now handled by SelectTool directly in mouseReleaseEvent
    // This prevents the two-step menu issue
    // If the active tool is SelectTool, just consume the event and do nothing
    // (SelectTool already handled it in mouseReleaseEvent)

    if (m_activeTool && m_activeTool->name() == "Select") {
        // SelectTool handles its own context menu in mouseReleaseEvent
        event->accept();  // Consume the event, do nothing
        return;
    }

    // For other tools, show a basic context menu
    event->accept();

    if (!m_document) {
        return;
    }

    QMenu menu(this);

    // Check if there are selected objects
    bool hasSelection = !m_document->selectedObjects().isEmpty();

    if (hasSelection) {
        // Add transformation tools for selected objects
        QAction* rotateAction = menu.addAction(tr("🔄 Rotate (Ctrl+R)"));
        connect(rotateAction, &QAction::triggered, [this]() {
            emit toolRequested("Rotate");
        });

        QAction* mirrorAction = menu.addAction(tr("↔️ Mirror (Ctrl+M)"));
        connect(mirrorAction, &QAction::triggered, [this]() {
            emit toolRequested("Mirror");
        });

        QAction* scaleAction = menu.addAction(tr("📐 Scale"));
        connect(scaleAction, &QAction::triggered, [this]() {
            emit toolRequested("Scale");
        });

        menu.addSeparator();

        QAction* deleteAction = menu.addAction(tr("🗑️ Delete (Del)"));
        connect(deleteAction, &QAction::triggered, [this]() {
            if (m_document) {
                auto selected = m_document->selectedObjects();
                for (auto* obj : selected) {
                    m_document->removeObject(obj);
                }
            }
        });
    } else {
        // No selection - inform user
        QAction* infoAction = menu.addAction(tr("No object selected"));
        infoAction->setEnabled(false);
    }

    menu.exec(event->globalPos());
}

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    // Draw white background
    painter->fillRect(rect, Qt::white);

    // Draw grid if visible
    if (m_gridVisible) {
        drawGrid(painter, rect);
    }

    // Draw origin indicator
    drawOriginIndicator(painter);
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
    // Base grid spacing in scene units (mm)
    double baseGridSpacing = 10.0;

    // Auto-scale grid based on zoom level to maintain readable spacing
    // Target: keep grid spacing between 10-50 pixels on screen
    double minVisiblePixels = 10.0;
    double maxVisiblePixels = 50.0;

    // Calculate actual grid spacing with auto-scaling
    double gridSpacing = baseGridSpacing;
    double screenSpacing = gridSpacing * m_zoomLevel;

    // If spacing too small, increase by powers of 2
    while (screenSpacing < minVisiblePixels && gridSpacing < 10000.0) {
        gridSpacing *= 2.0;
        screenSpacing = gridSpacing * m_zoomLevel;
    }

    // If spacing too large, decrease by powers of 2
    while (screenSpacing > maxVisiblePixels && gridSpacing > 0.1) {
        gridSpacing /= 2.0;
        screenSpacing = gridSpacing * m_zoomLevel;
    }

    // Calculate grid bounds
    int left = static_cast<int>(std::floor(rect.left() / gridSpacing));
    int right = static_cast<int>(std::ceil(rect.right() / gridSpacing));
    int top = static_cast<int>(std::floor(rect.top() / gridSpacing));
    int bottom = static_cast<int>(std::ceil(rect.bottom() / gridSpacing));

    // Draw grid lines with alpha based on spacing level
    int alpha = 100;
    if (gridSpacing != baseGridSpacing) {
        // Lighter color for scaled grid lines
        alpha = 60;
    }
    QPen gridPen(QColor(220, 220, 220, alpha));
    painter->setPen(gridPen);

    for (int i = left; i <= right; ++i) {
        double x = i * gridSpacing;
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    for (int i = top; i <= bottom; ++i) {
        double y = i * gridSpacing;
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }

    // Draw axes (always visible, darker)
    QPen axisPen(QColor(180, 180, 180), 2.0 / m_zoomLevel);
    painter->setPen(axisPen);
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));
}

void Canvas::drawOriginIndicator(QPainter* painter)
{
    // Draw a visible indicator at (0,0)
    painter->save();

    // Draw crosshair at origin
    double size = 20.0 / m_zoomLevel;  // Size scales with zoom
    QPen originPen(QColor(255, 0, 0), 2.0 / m_zoomLevel);  // Red, width scales with zoom
    painter->setPen(originPen);

    // Draw cross
    painter->drawLine(QPointF(-size, 0), QPointF(size, 0));
    painter->drawLine(QPointF(0, -size), QPointF(0, size));

    // Draw circle at center
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), size * 0.3, size * 0.3);

    painter->restore();
}

} // namespace UI
} // namespace PatternCAD
