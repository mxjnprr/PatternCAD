/**
 * GeometryObject.cpp
 *
 * Implementation of GeometryObject base class
 */

#include "GeometryObject.h"
#include <QUuid>

namespace PatternCAD {
namespace Geometry {

GeometryObject::GeometryObject(QObject* parent)
    : QObject(parent)
    , m_id(generateId())
    , m_name("Object")
    , m_layer("Default")
    , m_visible(true)
    , m_selected(false)
    , m_locked(false)
    , m_lineWeight(1.0)
    , m_lineColor(Qt::black)
    , m_lineStyle(LineStyle::Solid)
{
}

GeometryObject::~GeometryObject()
{
}

QString GeometryObject::id() const
{
    return m_id;
}

void GeometryObject::setId(const QString& id)
{
    if (m_id != id) {
        m_id = id;
        notifyChanged();
    }
}

QString GeometryObject::name() const
{
    return m_name;
}

void GeometryObject::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        notifyChanged();
    }
}

bool GeometryObject::isVisible() const
{
    return m_visible;
}

void GeometryObject::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        notifyChanged();
    }
}

bool GeometryObject::isSelected() const
{
    return m_selected;
}

void GeometryObject::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        emit selectionChanged(selected);
        notifyChanged();
    }
}

bool GeometryObject::isLocked() const
{
    return m_locked;
}

void GeometryObject::setLocked(bool locked)
{
    if (m_locked != locked) {
        m_locked = locked;
        notifyChanged();
    }
}

QString GeometryObject::layer() const
{
    return m_layer;
}

void GeometryObject::setLayer(const QString& layer)
{
    if (m_layer != layer) {
        m_layer = layer;
        notifyChanged();
    }
}

double GeometryObject::lineWeight() const
{
    return m_lineWeight;
}

void GeometryObject::setLineWeight(double weight)
{
    if (m_lineWeight != weight && weight >= 0.1 && weight <= 5.0) {
        m_lineWeight = weight;
        notifyChanged();
    }
}

QColor GeometryObject::lineColor() const
{
    return m_lineColor;
}

void GeometryObject::setLineColor(const QColor& color)
{
    if (m_lineColor != color) {
        m_lineColor = color;
        notifyChanged();
    }
}

GeometryObject::LineStyle GeometryObject::lineStyle() const
{
    return m_lineStyle;
}

void GeometryObject::setLineStyle(LineStyle style)
{
    if (m_lineStyle != style) {
        m_lineStyle = style;
        notifyChanged();
    }
}

void GeometryObject::notifyChanged()
{
    emit changed();
}

QString GeometryObject::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QPen GeometryObject::createPen(const QColor& layerColor) const
{
    // Use lineColor if set, otherwise use layer color
    QColor penColor = (m_lineColor != Qt::black) ? m_lineColor : layerColor;

    // Override with red if selected
    if (m_selected) {
        penColor = Qt::red;
    }

    // Create pen with lineWeight
    double penWidth = m_selected ? m_lineWeight + 1.0 : m_lineWeight;
    QPen pen(penColor, penWidth);

    // Set line style
    switch (m_lineStyle) {
        case LineStyle::Dashed:
            pen.setStyle(Qt::DashLine);
            break;
        case LineStyle::Dotted:
            pen.setStyle(Qt::DotLine);
            break;
        case LineStyle::Solid:
        default:
            pen.setStyle(Qt::SolidLine);
            break;
    }

    return pen;
}

} // namespace Geometry
} // namespace PatternCAD
