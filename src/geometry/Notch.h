/**
 * Notch.h
 *
 * Notch markers for pattern pieces - used to mark assembly points and alignment guides
 */

#ifndef PATTERNCAD_NOTCH_H
#define PATTERNCAD_NOTCH_H

#include <QObject>
#include <QPointF>
#include <QPainter>
#include <QJsonObject>

namespace PatternCAD {

namespace Geometry {
    class Polyline;
}

/**
 * Notch style enumeration
 */
enum class NotchStyle {
    VNotch,   // V-shaped cut (default)
    Slit,     // Straight cut perpendicular to edge
    Dot       // Small circle (marking only, not cut)
};

/**
 * Notch represents a marker on a pattern edge.
 * Used for indicating assembly points, alignment marks, and cut guides.
 */
class Notch : public QObject
{
    Q_OBJECT

public:
    explicit Notch(QObject* parent = nullptr);
    Notch(Geometry::Polyline* polyline, int segmentIndex, double position, 
          NotchStyle style = NotchStyle::VNotch, double depth = 5.0, 
          QObject* parent = nullptr);
    ~Notch();

    // Unique ID
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    // Source polyline
    Geometry::Polyline* polyline() const { return m_polyline; }
    void setPolyline(Geometry::Polyline* polyline);

    // Position on segment (0.0 - 1.0)
    int segmentIndex() const { return m_segmentIndex; }
    void setSegmentIndex(int index);

    double position() const { return m_position; }
    void setPosition(double pos);

    // Style
    NotchStyle style() const { return m_style; }
    void setStyle(NotchStyle style);

    // Depth in mm (default: 5mm)
    double depth() const { return m_depth; }
    void setDepth(double depth);

    // Calculated properties
    QPointF getLocation() const;
    QPointF getNormal() const;  // Outward-facing normal at this position

    // Rendering
    void render(QPainter* painter, const QColor& color = Qt::darkBlue) const;

    // Clone for duplication
    Notch* clone(Geometry::Polyline* newPolyline = nullptr) const;

    // Serialization
    QJsonObject toJson() const;
    static Notch* fromJson(const QJsonObject& json, Geometry::Polyline* polyline);

signals:
    void changed();

private:
    QString m_id;
    Geometry::Polyline* m_polyline;
    int m_segmentIndex;      // Index of the segment (edge) this notch is on
    double m_position;       // Position along segment (0.0 = start, 1.0 = end)
    NotchStyle m_style;
    double m_depth;          // Depth in mm

    // ID generation
    QString generateId() const;

    // Rendering helpers
    void renderVNotch(QPainter* painter, const QPointF& loc, const QPointF& normal) const;
    void renderSlit(QPainter* painter, const QPointF& loc, const QPointF& normal) const;
    void renderDot(QPainter* painter, const QPointF& loc) const;
};

} // namespace PatternCAD

#endif // PATTERNCAD_NOTCH_H
