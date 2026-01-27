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
    , m_visible(true)
    , m_selected(false)
    , m_locked(false)
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

void GeometryObject::notifyChanged()
{
    emit changed();
}

QString GeometryObject::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

} // namespace Geometry
} // namespace PatternCAD
