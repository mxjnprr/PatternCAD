/**
 * NotchTool.h
 *
 * Tool for placing and managing notches on pattern edges
 */

#ifndef PATTERNCAD_NOTCHTOOL_H
#define PATTERNCAD_NOTCHTOOL_H

#include "Tool.h"
#include "geometry/Notch.h"
#include <QPointF>
#include <QMenu>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Polyline;
}

class Notch;

namespace Tools {

/**
 * NotchTool allows users to:
 * - Click on pattern edges to place notches
 * - Select notch style (V-notch, slit, dot)
 * - Configure notch depth
 * - Drag to reposition notches
 * - Delete notches with Delete key
 * - Right-click for context menu
 * - Keyboard shortcut: 'N'
 */
class NotchTool : public Tool
{
    Q_OBJECT

public:
    explicit NotchTool(QObject* parent = nullptr);
    ~NotchTool();

    // Tool identification
    QString name() const override;
    QString description() const override;
    QCursor cursor() const override;

    // Tool lifecycle
    void activate() override;
    void deactivate() override;
    void reset() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Drawing overlay
    void drawOverlay(QPainter* painter) override;

    // Configuration
    void setNotchStyle(NotchStyle style);
    void setNotchDepth(double depth);
    NotchStyle notchStyle() const { return m_notchStyle; }
    double notchDepth() const { return m_notchDepth; }

signals:
    void notchStyleChanged(NotchStyle style);
    void notchDepthChanged(double depth);
    void notchSelected(Notch* notch);

private:
    enum class Mode {
        Idle,           // Ready to place or select
        Dragging,       // Dragging an existing notch
        PreviewPlace    // Showing where notch will be placed
    };

    Mode m_mode;
    NotchStyle m_notchStyle;     // Current notch style for new notches
    double m_notchDepth;         // Current notch depth (mm)

    // Current interaction state
    Geometry::Polyline* m_hoveredPolyline;   // Polyline under mouse
    int m_hoveredSegment;                      // Segment index under mouse
    double m_hoveredPosition;                  // Position on segment (0.0-1.0)
    QPointF m_hoveredPoint;                    // World position of hover point

    // Selection and dragging
    Notch* m_selectedNotch;                    // Currently selected notch
    Geometry::Polyline* m_selectedPolyline;   // Polyline containing selected notch
    Notch* m_dragNotch;                        // Notch being dragged
    Geometry::Polyline* m_dragPolyline;       // Polyline of notch being dragged

    // Context menu
    QMenu* m_contextMenu;

    // Helper methods
    Geometry::Polyline* findPolylineAt(const QPointF& point, int* segmentIndex = nullptr,
                                        double* position = nullptr) const;
    Notch* findNotchAt(const QPointF& point, Geometry::Polyline** polyline = nullptr) const;
    void placeNotch();
    void deleteSelectedNotch();
    void selectNotch(Notch* notch, Geometry::Polyline* polyline);
    void deselectNotch();
    void showContextMenu(const QPoint& screenPos);
    void updateStatusMessage();
    void drawPreviewNotch(QPainter* painter) const;
    void drawNotchHighlight(QPainter* painter, Notch* notch, Geometry::Polyline* polyline) const;

private slots:
    void onStyleVNotch();
    void onStyleSlit();
    void onStyleDot();
    void onDeleteNotch();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_NOTCHTOOL_H
