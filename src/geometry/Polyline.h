/**
 * Polyline.h
 *
 * Closed polyline with sharp and smooth vertices
 */

#ifndef PATTERNCAD_POLYLINE_H
#define PATTERNCAD_POLYLINE_H

#include "GeometryObject.h"
#include <QPointF>
#include <QVector>

namespace PatternCAD {

// Forward declaration
class SeamAllowance;

namespace Geometry {

/**
 * Vertex type for polyline
 */
enum class VertexType {
    Sharp,   // Point pointu (coin)
    Smooth   // Point courbe (tangente continue)
};

/**
 * Vertex in a polyline with position and type
 */
struct PolylineVertex {
    QPointF position;
    VertexType type;
    double incomingTension;  // For smooth vertices: tension for incoming handle
    double outgoingTension;  // For smooth vertices: tension for outgoing handle
    QPointF tangent;         // For smooth vertices: direction of the tangent (outgoing)

    PolylineVertex(const QPointF& pos = QPointF(), VertexType t = VertexType::Sharp,
                   double inTens = 0.5, double outTens = 0.5, const QPointF& tang = QPointF())
        : position(pos), type(t), incomingTension(inTens), outgoingTension(outTens), tangent(tang) {}

    // Legacy constructor for backward compatibility
    PolylineVertex(const QPointF& pos, VertexType t, double tens, const QPointF& tang)
        : position(pos), type(t), incomingTension(tens), outgoingTension(tens), tangent(tang) {}
};

/**
 * Polyline represents a closed polygonal line with mixed vertex types:
 * - Sharp vertices (corners)
 * - Smooth vertices (curves passing through the point)
 */
class Polyline : public GeometryObject
{
    Q_OBJECT

public:
    explicit Polyline(QObject* parent = nullptr);
    explicit Polyline(const QVector<PolylineVertex>& vertices, QObject* parent = nullptr);
    ~Polyline() override;

    // Type identification
    ObjectType type() const override { return ObjectType::Polyline; }
    QString typeName() const override { return "Polyline"; }

    // Vertices
    QVector<PolylineVertex> vertices() const { return m_vertices; }
    void setVertices(const QVector<PolylineVertex>& vertices);
    void addVertex(const QPointF& position, VertexType type = VertexType::Sharp,
                   double tension = 0.5, const QPointF& tangent = QPointF());
    void addVertex(const PolylineVertex& vertex);
    int vertexCount() const { return m_vertices.size(); }
    void clear() { m_vertices.clear(); notifyChanged(); }

    // Vertex manipulation
    void insertVertex(int index, const PolylineVertex& vertex);
    void removeVertex(int index);
    void updateVertex(int index, const QPointF& position);
    void setVertexType(int index, VertexType type);
    PolylineVertex vertexAt(int index) const;

    // Vertex hit testing
    int findVertexAt(const QPointF& point, double tolerance = 5.0) const;
    int findClosestSegment(const QPointF& point, QPointF* closestPoint = nullptr) const;

    // Segment length calculation (accounts for curves)
    double calculateSegmentLength(int segmentIndex) const;
    double calculateSegmentLength(int segmentIndex, int overrideVertexIndex, const QPointF& overridePosition) const;

    // Closed property
    bool isClosed() const { return m_closed; }
    void setClosed(bool closed);

    // Seam allowance
    SeamAllowance* seamAllowance() const { return m_seamAllowance; }

    // Geometry interface
    QRectF boundingRect() const override;
    bool contains(const QPointF& point) const override;
    void translate(const QPointF& delta) override;
    void rotate(double angleDegrees, const QPointF& center) override;
    void mirror(const QPointF& axisPoint1, const QPointF& axisPoint2) override;
    void scale(double scaleX, double scaleY, const QPointF& origin) override;

    // Drawing
    void draw(QPainter* painter, const QColor& color = Qt::black) const override;

private:
    QVector<PolylineVertex> m_vertices;
    bool m_closed;
    SeamAllowance* m_seamAllowance;

    // Helper methods
    QPainterPath createPath() const;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_POLYLINE_H
