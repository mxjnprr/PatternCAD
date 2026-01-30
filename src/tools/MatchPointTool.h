/**
 * MatchPointTool.h
 *
 * Tool for placing and managing match points on pattern edges
 */

#ifndef PATTERNCAD_MATCHPOINTTOOL_H
#define PATTERNCAD_MATCHPOINTTOOL_H

#include "Tool.h"
#include <QPointF>
#include <QMenu>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Polyline;
}

class MatchPoint;

namespace Tools {

/**
 * MatchPointTool allows users to:
 * - Click on pattern edges to place match points
 * - Enter labels for match points
 * - Drag to reposition match points
 * - Shift+click to link/unlink match points between pieces
 * - Delete match points with Delete key
 * - Right-click for context menu
 * - Keyboard shortcut: 'M'
 */
class MatchPointTool : public Tool
{
    Q_OBJECT

public:
    explicit MatchPointTool(QObject* parent = nullptr);
    ~MatchPointTool();

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
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Drawing overlay
    void drawOverlay(QPainter* painter) override;

    // Configuration
    void setDefaultLabel(const QString& label);
    QString defaultLabel() const { return m_defaultLabel; }

signals:
    void matchPointSelected(MatchPoint* matchPoint);
    void linkingStarted(MatchPoint* from);
    void linkingCompleted(MatchPoint* from, MatchPoint* to);

private:
    enum class Mode {
        Idle,           // Ready to place or select
        Dragging,       // Dragging an existing match point
        PreviewPlace,   // Showing where match point will be placed
        Linking         // Linking two match points together
    };

    Mode m_mode;
    QString m_defaultLabel;          // Next label to use
    int m_labelCounter;              // Counter for auto-generated labels

    // Current interaction state
    Geometry::Polyline* m_hoveredPolyline;   // Polyline under mouse
    int m_hoveredSegment;                      // Segment index under mouse
    double m_hoveredPosition;                  // Position on segment (0.0-1.0)
    QPointF m_hoveredPoint;                    // World position of hover point

    // Selection and dragging
    MatchPoint* m_selectedMatchPoint;          // Currently selected match point
    Geometry::Polyline* m_selectedPolyline;   // Polyline containing selected match point
    MatchPoint* m_dragMatchPoint;              // Match point being dragged
    Geometry::Polyline* m_dragPolyline;       // Polyline of match point being dragged

    // Linking mode
    MatchPoint* m_linkSourcePoint;             // Source point for linking
    Geometry::Polyline* m_linkSourcePolyline; // Source polyline for linking

    // Context menu
    QMenu* m_contextMenu;

    // Helper methods
    Geometry::Polyline* findPolylineAt(const QPointF& point, int* segmentIndex = nullptr,
                                        double* position = nullptr) const;
    MatchPoint* findMatchPointAt(const QPointF& point, Geometry::Polyline** polyline = nullptr) const;
    void placeMatchPoint();
    void deleteSelectedMatchPoint();
    void selectMatchPoint(MatchPoint* matchPoint, Geometry::Polyline* polyline);
    void deselectMatchPoint();
    void startLinking();
    void completeLinking(MatchPoint* target, Geometry::Polyline* targetPolyline);
    void cancelLinking();
    void editLabel();
    void showContextMenu(const QPoint& screenPos);
    void updateStatusMessage();
    QString generateNextLabel();
    QString peekNextLabel() const;  // For preview without incrementing
    void drawPreviewMatchPoint(QPainter* painter) const;
    void drawMatchPointHighlight(QPainter* painter, MatchPoint* matchPoint, 
                                  Geometry::Polyline* polyline) const;
    void drawLinkingLine(QPainter* painter) const;

private slots:
    void onEditLabel();
    void onLinkMatchPoint();
    void onUnlinkMatchPoint();
    void onDeleteMatchPoint();
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_MATCHPOINTTOOL_H
