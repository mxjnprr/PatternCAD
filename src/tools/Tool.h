/**
 * Tool.h
 *
 * Base class for all drawing and editing tools
 */

#ifndef PATTERNCAD_TOOL_H
#define PATTERNCAD_TOOL_H

#include <QObject>
#include <QString>
#include <QCursor>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

namespace PatternCAD {

// Forward declarations
class Document;

namespace UI {
    class Canvas;
}

namespace Tools {

/**
 * Tool state enumeration
 */
enum class ToolState {
    Idle,       // Ready to start
    Active,     // Currently in use
    Preview     // Showing preview
};

/**
 * Base class for all tools providing:
 * - Mouse event handling
 * - Keyboard event handling
 * - Tool activation/deactivation
 * - Visual feedback
 * - Cursor management
 */
class Tool : public QObject
{
    Q_OBJECT

public:
    explicit Tool(QObject* parent = nullptr);
    virtual ~Tool();

    // Tool identification
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QCursor cursor() const;

    // Tool state
    ToolState state() const;
    bool isActive() const;

    // Canvas and document
    UI::Canvas* canvas() const;
    void setCanvas(UI::Canvas* canvas);

    Document* document() const;
    void setDocument(Document* document);

    // Tool lifecycle
    virtual void activate();
    virtual void deactivate();
    virtual void reset();

    // Event handlers
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    // Drawing overlay
    virtual void drawOverlay(QPainter* painter);

signals:
    void stateChanged(ToolState state);
    void statusMessage(const QString& message);

protected:
    void setState(ToolState state);

    // Helper methods
    QPointF snapToGrid(const QPointF& point) const;
    void showStatusMessage(const QString& message);

    // Protected members
    ToolState m_state;
    UI::Canvas* m_canvas;
    Document* m_document;
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_TOOL_H
