/**
 * SelectTool.h
 *
 * Selection tool for selecting and manipulating objects
 */

#ifndef PATTERNCAD_SELECTTOOL_H
#define PATTERNCAD_SELECTTOOL_H

#include "Tool.h"
#include "ui/DimensionInputOverlay.h"
#include <QPointF>
#include <QRectF>
#include <QList>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Polyline;
}

namespace Tools {

/**
 * SelectTool provides selection functionality:
 * - Click to select single objects
 * - Click-drag for rectangular selection
 * - Ctrl+Click for multi-selection
 * - Move selected objects by dragging
 * - Display selection handles
 */
class SelectTool : public Tool
{
    Q_OBJECT

public:
    explicit SelectTool(QObject* parent = nullptr);
    ~SelectTool();

    // Tool identification
    QString name() const override;
    QString description() const override;
    QCursor cursor() const override;

    // Tool lifecycle
    void activate() override;
    void reset() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Drawing overlay
    void drawOverlay(QPainter* painter) override;

    // Apply dimension to free segment when one is locked
    void applyFreeSegmentLength(double lengthInMm, double angleDegrees);

    // Apply dimension to selected segment
    void applySegmentLength(double lengthInMm, double angleDegrees, UI::ResizeMode mode);

    // Check if a vertex is currently targeted (selected or hovered)
    bool hasVertexTargeted() const;

    // Segment identification for multi-segment locking
    struct SegmentKey {
        Geometry::GeometryObject* object;
        int segmentIndex;

        bool operator==(const SegmentKey& other) const {
            return object == other.object && segmentIndex == other.segmentIndex;
        }

        bool operator<(const SegmentKey& other) const {
            if (object != other.object)
                return object < other.object;
            return segmentIndex < other.segmentIndex;
        }

        friend size_t qHash(const SegmentKey& key, size_t seed = 0) noexcept {
            return qHash(reinterpret_cast<quintptr>(key.object), seed) ^ qHash(key.segmentIndex, seed);
        }
    };

private:
    enum class SelectMode {
        None,
        Hovering,       // Hovering to highlight object
        Dragging,       // Dragging selected object with mouse
        VertexSelected, // Vertex selected but not moving yet
        DraggingVertex, // Dragging a vertex
        DraggingHandle, // Dragging a curve handle
        Selecting       // Drawing selection rectangle
    };

    SelectMode m_mode;
    QPointF m_startPoint;
    QPointF m_currentPoint;
    QPointF m_lastPoint;
    QRectF m_selectionRect;
    bool m_multiSelect;
    Geometry::GeometryObject* m_hoveredObject;
    QPointF m_dragOffset;  // Offset from object center to mouse when starting drag

    // Vertex selection
    Geometry::GeometryObject* m_selectedVertexObject;  // Object containing selected vertex
    int m_selectedVertexIndex;                          // Index of selected vertex
    int m_hoveredVertexIndex;                           // Index of hovered vertex (-1 if none)
    QPointF m_vertexStartPosition;                      // Initial position of vertex when drag starts (for undo)

    // Segment selection
    Geometry::GeometryObject* m_selectedSegmentObject;  // Object containing selected segment
    int m_selectedSegmentIndex;                         // Index of selected segment
    int m_hoveredSegmentIndex;                          // Index of hovered segment (-1 if none)

    // Curve handle manipulation
    Geometry::GeometryObject* m_selectedHandleObject;   // Object containing the handle's vertex
    int m_selectedHandleVertexIndex;                    // Index of vertex with selected handle
    int m_selectedHandleSide;                           // -1 = incoming, +1 = outgoing
    int m_hoveredHandleVertexIndex;                     // Index of hovered handle vertex (-1 if none)
    int m_hoveredHandleSide;                            // -1 = incoming, +1 = outgoing, 0 = none
    QPointF m_handleStartTangent;                       // Initial tangent for undo
    double m_handleStartTension;                        // Initial tension for undo

    // Length constraints (single segment - legacy)
    int m_constrainedSegmentIndex;     // Index of constrained segment (-1 = none)
    double m_constrainedLength;        // Locked length value
    QPointF m_constrainedSegmentStart; // Start point of locked segment
    QPointF m_constrainedSegmentEnd;   // End point of locked segment

    // Multi-segment lock (new)
    QSet<SegmentKey> m_selectedSegments;     // Segments marked for locking (orange)
    QSet<SegmentKey> m_lockedSegments;       // Segments locked (red with lock icon)
    QMap<SegmentKey, double> m_lockedLengths;
    QMap<SegmentKey, QPair<QPointF, QPointF>> m_lockedSegmentPositions;

    // Helper methods
    void selectObjectAt(const QPointF& point, bool addToSelection);
    void selectObjectsInRect(const QRectF& rect, bool addToSelection);
    void startMoving();
    void updateMove(const QPointF& delta);
    void finishMove();
    Geometry::GeometryObject* findObjectAt(const QPointF& point) const;
    QList<Geometry::GeometryObject*> findObjectsInRect(const QRectF& rect) const;
    void showContextMenu(const QPoint& globalPos);

    // Vertex manipulation
    int findVertexAt(const QPointF& point, Geometry::GeometryObject** outObject) const;
    void drawVertexHandles(QPainter* painter, Geometry::GeometryObject* obj) const;

    // Segment manipulation
    int findSegmentAt(const QPointF& point, Geometry::GeometryObject** outObject) const;

    // Curve handle manipulation
    int findHandleAt(const QPointF& point, Geometry::GeometryObject** outObject, int* outSide) const;
    void drawCurveHandles(QPainter* painter, Geometry::GeometryObject* obj, int vertexIndex) const;
    QPointF getHandlePosition(Geometry::Polyline* polyline, int vertexIndex, int side) const;

    // Multi-segment lock helpers
    void toggleSegmentSelection(Geometry::GeometryObject* obj, int segmentIdx);
    void lockSelectedSegments();
    void unlockSegment(Geometry::GeometryObject* obj, int segmentIdx);
    void unlockAllSegments();
    bool isSegmentLocked(Geometry::GeometryObject* obj, int segmentIdx) const;
    bool isSegmentSelected(Geometry::GeometryObject* obj, int segmentIdx) const;
    void drawSelectedSegments(QPainter* painter) const;
    void drawLockedSegments(QPainter* painter) const;
    QPointF constrainVertexWithLockedSegments(const QPointF& newPos, Geometry::Polyline* polyline, int vertexIndex) const;
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_SELECTTOOL_H
