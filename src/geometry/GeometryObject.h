/**
 * GeometryObject.h
 *
 * Base class for all geometric objects in the pattern
 */

#ifndef PATTERNCAD_GEOMETRYOBJECT_H
#define PATTERNCAD_GEOMETRYOBJECT_H

#include <QObject>
#include <QString>
#include <QPointF>
#include <QRectF>
#include <QPainter>
#include <memory>

namespace PatternCAD {
namespace Geometry {

/**
 * GeometryObject type enumeration
 */
enum class ObjectType {
    Point,
    Line,
    Circle,
    Rectangle,
    CubicBezier,
    Arc,
    Polyline,
    Polygon
};

/**
 * Base class for all geometric objects providing:
 * - Common properties (position, rotation, visibility)
 * - Selection and highlighting
 * - Bounding box calculation
 * - Drawing interface
 * - Serialization
 */
class GeometryObject : public QObject
{
    Q_OBJECT

public:
    explicit GeometryObject(QObject* parent = nullptr);
    virtual ~GeometryObject();

    // Type identification
    virtual ObjectType type() const = 0;
    virtual QString typeName() const = 0;

    // Object properties
    QString id() const;
    void setId(const QString& id);

    QString name() const;
    void setName(const QString& name);

    bool isVisible() const;
    void setVisible(bool visible);

    bool isSelected() const;
    void setSelected(bool selected);

    bool isLocked() const;
    void setLocked(bool locked);

    // Geometry
    virtual QRectF boundingRect() const = 0;
    virtual bool contains(const QPointF& point) const = 0;
    virtual void translate(const QPointF& delta) = 0;

    // Drawing
    virtual void draw(QPainter* painter) const = 0;

    // Serialization (to be implemented)
    // virtual QJsonObject toJson() const = 0;
    // virtual void fromJson(const QJsonObject& json) = 0;

signals:
    void changed();
    void selectionChanged(bool selected);

protected:
    QString m_id;
    QString m_name;
    bool m_visible;
    bool m_selected;
    bool m_locked;

    // Helper methods
    void notifyChanged();
    QString generateId() const;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_GEOMETRYOBJECT_H
