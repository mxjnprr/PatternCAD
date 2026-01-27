/**
 * Tool.cpp
 *
 * Implementation of Tool base class
 */

#include "Tool.h"
#include "ui/Canvas.h"
#include "core/Document.h"

namespace PatternCAD {
namespace Tools {

Tool::Tool(QObject* parent)
    : QObject(parent)
    , m_state(ToolState::Idle)
    , m_canvas(nullptr)
    , m_document(nullptr)
{
}

Tool::~Tool()
{
}

QCursor Tool::cursor() const
{
    return QCursor(Qt::CrossCursor);
}

ToolState Tool::state() const
{
    return m_state;
}

bool Tool::isActive() const
{
    return m_state != ToolState::Idle;
}

UI::Canvas* Tool::canvas() const
{
    return m_canvas;
}

void Tool::setCanvas(UI::Canvas* canvas)
{
    m_canvas = canvas;
}

Document* Tool::document() const
{
    return m_document;
}

void Tool::setDocument(Document* document)
{
    m_document = document;
}

void Tool::activate()
{
    setState(ToolState::Idle);
    // TODO: Set cursor on canvas
}

void Tool::deactivate()
{
    reset();
}

void Tool::reset()
{
    setState(ToolState::Idle);
}

void Tool::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // Base implementation does nothing
}

void Tool::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // Base implementation does nothing
}

void Tool::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // Base implementation does nothing
}

void Tool::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // Base implementation does nothing
}

void Tool::keyPressEvent(QKeyEvent* event)
{
    // Handle common keyboard shortcuts
    if (event->key() == Qt::Key_Escape) {
        reset();
        event->accept();
    }
}

void Tool::keyReleaseEvent(QKeyEvent* event)
{
    Q_UNUSED(event);
    // Base implementation does nothing
}

void Tool::drawOverlay(QPainter* painter)
{
    Q_UNUSED(painter);
    // Base implementation does nothing
}

void Tool::setState(ToolState state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(state);
    }
}

QPointF Tool::snapToGrid(const QPointF& point) const
{
    if (m_canvas) {
        return m_canvas->snapPoint(point);
    }
    return point;
}

void Tool::showStatusMessage(const QString& message)
{
    emit statusMessage(message);
}

} // namespace Tools
} // namespace PatternCAD
