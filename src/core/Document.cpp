/**
 * Document.cpp
 *
 * Implementation of Document class
 */

#include "Document.h"
#include "geometry/GeometryObject.h"

namespace PatternCAD {

Document::Document(QObject* parent)
    : QObject(parent)
    , m_name("Untitled")
    , m_modified(false)
    , m_activeLayer("Default")
{
    // Add default layer
    m_layers.append("Default");
}

Document::~Document()
{
    // Cleanup objects
    qDeleteAll(m_objects);
    m_objects.clear();
}

QString Document::name() const
{
    return m_name;
}

void Document::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
    }
}

bool Document::isModified() const
{
    return m_modified;
}

void Document::setModified(bool modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(modified);
    }
}

void Document::addObject(Geometry::GeometryObject* object)
{
    if (object && !m_objects.contains(object)) {
        m_objects.append(object);
        emit objectAdded(object);
        notifyModified();
    }
}

void Document::removeObject(Geometry::GeometryObject* object)
{
    if (object && m_objects.contains(object)) {
        m_objects.removeAll(object);
        m_selectedObjects.removeAll(object);
        emit objectRemoved(object);
        notifyModified();
        delete object;
    }
}

QList<Geometry::GeometryObject*> Document::objects() const
{
    return m_objects;
}

QList<Geometry::GeometryObject*> Document::objectsOnLayer(const QString& layerName) const
{
    QList<Geometry::GeometryObject*> result;

    // TODO: Filter objects by layer when layer support is added to GeometryObject
    // for (auto* obj : m_objects) {
    //     if (obj->layer() == layerName) {
    //         result.append(obj);
    //     }
    // }

    return result;
}

QList<Geometry::GeometryObject*> Document::selectedObjects() const
{
    return m_selectedObjects;
}

void Document::setSelectedObjects(const QList<Geometry::GeometryObject*>& objects)
{
    m_selectedObjects = objects;
    emit selectionChanged();
}

void Document::clearSelection()
{
    if (!m_selectedObjects.isEmpty()) {
        m_selectedObjects.clear();
        emit selectionChanged();
    }
}

void Document::selectAll()
{
    m_selectedObjects = m_objects;
    emit selectionChanged();
}

QStringList Document::layers() const
{
    return m_layers;
}

void Document::addLayer(const QString& layerName)
{
    if (!m_layers.contains(layerName)) {
        m_layers.append(layerName);
        emit layerAdded(layerName);
        notifyModified();
    }
}

void Document::removeLayer(const QString& layerName)
{
    if (m_layers.contains(layerName) && m_layers.size() > 1) {
        m_layers.removeAll(layerName);
        emit layerRemoved(layerName);

        // If active layer was removed, switch to first layer
        if (m_activeLayer == layerName) {
            setActiveLayer(m_layers.first());
        }

        notifyModified();
    }
}

void Document::renameLayer(const QString& oldName, const QString& newName)
{
    int index = m_layers.indexOf(oldName);
    if (index >= 0 && !m_layers.contains(newName)) {
        m_layers[index] = newName;
        emit layerRenamed(oldName, newName);

        if (m_activeLayer == oldName) {
            m_activeLayer = newName;
            emit activeLayerChanged(newName);
        }

        notifyModified();
    }
}

QString Document::activeLayer() const
{
    return m_activeLayer;
}

void Document::setActiveLayer(const QString& layerName)
{
    if (m_layers.contains(layerName) && m_activeLayer != layerName) {
        m_activeLayer = layerName;
        emit activeLayerChanged(layerName);
    }
}

bool Document::save(const QString& filepath)
{
    // TODO: Implement save using FileFormat
    Q_UNUSED(filepath);
    return false;
}

bool Document::load(const QString& filepath)
{
    // TODO: Implement load using FileFormat
    Q_UNUSED(filepath);
    return false;
}

void Document::clear()
{
    // Clear all objects
    qDeleteAll(m_objects);
    m_objects.clear();
    m_selectedObjects.clear();

    // Reset layers to default
    m_layers.clear();
    m_layers.append("Default");
    m_activeLayer = "Default";

    // Reset document properties
    m_name = "Untitled";
    setModified(false);

    emit selectionChanged();
}

void Document::notifyModified()
{
    if (!m_modified) {
        setModified(true);
    }
}

} // namespace PatternCAD
