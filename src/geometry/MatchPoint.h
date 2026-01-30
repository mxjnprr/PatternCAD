/**
 * MatchPoint.h
 *
 * Match point markers for pattern assembly - indicates alignment points between pieces
 */

#ifndef PATTERNCAD_MATCHPOINT_H
#define PATTERNCAD_MATCHPOINT_H

#include <QObject>
#include <QPointF>
#include <QPainter>
#include <QVector>
#include <QJsonObject>

namespace PatternCAD {

namespace Geometry {
    class Polyline;
}

/**
 * MatchPoint represents an assembly alignment marker on a pattern.
 * Match points can be linked between different pattern pieces to show
 * which points should align during assembly.
 */
class MatchPoint : public QObject
{
    Q_OBJECT

public:
    explicit MatchPoint(QObject* parent = nullptr);
    MatchPoint(const QString& label, const QPointF& position, 
               Geometry::Polyline* polyline = nullptr,
               QObject* parent = nullptr);
    MatchPoint(const QString& label, Geometry::Polyline* polyline, 
               int segmentIndex, double segmentPosition,
               QObject* parent = nullptr);
    ~MatchPoint();

    // Unique ID
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    // Label (e.g., "A", "B", "1", "2")
    QString label() const { return m_label; }
    void setLabel(const QString& label);

    // Position - either absolute or relative to edge
    QPointF position() const;
    void setPosition(const QPointF& pos);

    // Edge-relative positioning (optional)
    bool isOnEdge() const { return m_isOnEdge; }
    Geometry::Polyline* polyline() const { return m_polyline; }
    void setPolyline(Geometry::Polyline* polyline);
    int segmentIndex() const { return m_segmentIndex; }
    void setSegmentIndex(int index);
    double segmentPosition() const { return m_segmentPosition; }
    void setSegmentPosition(double pos);

    // Linking to other match points
    QVector<MatchPoint*> linkedPoints() const { return m_linkedPoints; }
    void linkTo(MatchPoint* other);
    void unlinkFrom(MatchPoint* other);
    void unlinkAll();
    bool isLinkedTo(MatchPoint* other) const;

    // Rendering
    void render(QPainter* painter, const QColor& color = Qt::darkMagenta) const;
    void renderLinks(QPainter* painter, const QColor& color = Qt::darkGray) const;

    // Clone for duplication
    MatchPoint* clone(Geometry::Polyline* newPolyline = nullptr) const;

    // Serialization
    QJsonObject toJson() const;
    static MatchPoint* fromJson(const QJsonObject& json, Geometry::Polyline* polyline = nullptr);

signals:
    void changed();
    void linkChanged();

private:
    QString m_id;
    QString m_label;
    
    // Absolute position (if not on edge)
    QPointF m_absolutePosition;
    
    // Edge-relative position
    bool m_isOnEdge;
    Geometry::Polyline* m_polyline;
    int m_segmentIndex;
    double m_segmentPosition;  // 0.0-1.0 along segment
    
    // Linked match points
    QVector<MatchPoint*> m_linkedPoints;

    // ID generation
    QString generateId() const;

    // Calculate position on edge
    QPointF calculateEdgePosition() const;
};

} // namespace PatternCAD

#endif // PATTERNCAD_MATCHPOINT_H
