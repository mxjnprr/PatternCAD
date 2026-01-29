/**
 * Document.cpp
 *
 * Implementation of Document class
 */

#include "Document.h"
#include "Commands.h"
#include "geometry/GeometryObject.h"
#include "io/NativeFormat.h"
#include <QDebug>

namespace PatternCAD {

Document::Document(QObject* parent)
    : QObject(parent)
    , m_name("Untitled")
    , m_modified(false)
    , m_activeLayer("Default")
    , m_undoStack(new QUndoStack(this))
{
    // Add default layer with black color
    m_layers.append("Default");
    m_layerVisibility["Default"] = true;
    m_layerColors["Default"] = Qt::black;

    // Connect undo stack signals to document modified state
    connect(m_undoStack, &QUndoStack::indexChanged, this, [this]() {
        notifyModified();
    });
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
        // Assign object to active layer
        object->setLayer(m_activeLayer);

        // Use command for undo/redo support
        AddObjectCommand* cmd = new AddObjectCommand(this, object);
        m_undoStack->push(cmd);
    }
}

void Document::removeObject(Geometry::GeometryObject* object)
{
    if (object && m_objects.contains(object)) {
        // Use command for undo/redo support
        RemoveObjectCommand* cmd = new RemoveObjectCommand(this, object);
        m_undoStack->push(cmd);
    }
}

void Document::removeObjects(const QList<Geometry::GeometryObject*>& objects)
{
    if (objects.isEmpty()) {
        return;
    }

    // Filter to only include objects that are in the document
    QList<Geometry::GeometryObject*> validObjects;
    for (auto* obj : objects) {
        if (obj && m_objects.contains(obj)) {
            validObjects.append(obj);
        }
    }

    if (validObjects.isEmpty()) {
        return;
    }

    // Use command for undo/redo support
    RemoveObjectsCommand* cmd = new RemoveObjectsCommand(this, validObjects);
    m_undoStack->push(cmd);
}

QList<Geometry::GeometryObject*> Document::objects() const
{
    return m_objects;
}

QList<Geometry::GeometryObject*> Document::objectsOnLayer(const QString& layerName) const
{
    QList<Geometry::GeometryObject*> result;

    for (auto* obj : m_objects) {
        if (obj && obj->layer() == layerName) {
            result.append(obj);
        }
    }

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

void Document::addLayer(const QString& layerName, const QColor& color)
{
    if (!m_layers.contains(layerName)) {
        m_layers.append(layerName);
        m_layerVisibility[layerName] = true;
        m_layerColors[layerName] = color.isValid() ? color : Qt::black;
        emit layerAdded(layerName);
        notifyModified();
    }
}

void Document::removeLayer(const QString& layerName)
{
    if (m_layers.contains(layerName) && m_layers.size() > 1) {
        m_layers.removeAll(layerName);
        m_layerVisibility.remove(layerName);
        m_layerColors.remove(layerName);

        // Move objects from removed layer to first layer
        QString targetLayer = m_layers.first();
        for (auto* obj : m_objects) {
            if (obj && obj->layer() == layerName) {
                obj->setLayer(targetLayer);
            }
        }

        emit layerRemoved(layerName);

        // If active layer was removed, switch to first layer
        if (m_activeLayer == layerName) {
            setActiveLayer(targetLayer);
        }

        notifyModified();
    }
}

void Document::renameLayer(const QString& oldName, const QString& newName)
{
    int index = m_layers.indexOf(oldName);
    if (index >= 0 && !m_layers.contains(newName)) {
        m_layers[index] = newName;

        // Update visibility map
        bool visible = m_layerVisibility.value(oldName, true);
        m_layerVisibility.remove(oldName);
        m_layerVisibility[newName] = visible;

        // Update color map
        QColor color = m_layerColors.value(oldName, Qt::black);
        m_layerColors.remove(oldName);
        m_layerColors[newName] = color;

        // Update objects on this layer
        for (auto* obj : m_objects) {
            if (obj && obj->layer() == oldName) {
                obj->setLayer(newName);
            }
        }

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

bool Document::isLayerVisible(const QString& layerName) const
{
    return m_layerVisibility.value(layerName, true);
}

void Document::setLayerVisible(const QString& layerName, bool visible)
{
    qDebug() << "Document::setLayerVisible - layer:" << layerName << "visible:" << visible << "contains:" << m_layers.contains(layerName);
    if (m_layers.contains(layerName)) {
        bool oldVisible = m_layerVisibility.value(layerName, true);
        qDebug() << "  oldVisible:" << oldVisible << "new:" << visible;
        if (oldVisible != visible) {
            m_layerVisibility[layerName] = visible;
            qDebug() << "  Emitting layerVisibilityChanged signal";
            emit layerVisibilityChanged(layerName, visible);
        }
    }
}

QColor Document::layerColor(const QString& layerName) const
{
    return m_layerColors.value(layerName, Qt::black);
}

void Document::setLayerColor(const QString& layerName, const QColor& color)
{
    if (m_layers.contains(layerName) && color.isValid()) {
        m_layerColors[layerName] = color;
        emit layerVisibilityChanged(layerName, m_layerVisibility.value(layerName, true));
        notifyModified();
    }
}

bool Document::isLayerLocked(const QString& layerName) const
{
    return m_layerLocked.value(layerName, false);
}

void Document::setLayerLocked(const QString& layerName, bool locked)
{
    if (m_layers.contains(layerName)) {
        bool oldLocked = m_layerLocked.value(layerName, false);
        if (oldLocked != locked) {
            m_layerLocked[layerName] = locked;
            notifyModified();
        }
    }
}

bool Document::save(const QString& filepath)
{
    IO::NativeFormat format;
    bool success = format.exportFile(filepath, this);

    if (success) {
        setModified(false);
    }

    return success;
}

bool Document::load(const QString& filepath)
{
    IO::NativeFormat format;
    bool success = format.importFile(filepath, this);

    if (success) {
        setModified(false);
    }

    return success;
}

void Document::clear()
{
    // Clear all objects
    qDeleteAll(m_objects);
    m_objects.clear();
    m_selectedObjects.clear();

    // Reset layers to default
    m_layers.clear();
    m_layerVisibility.clear();
    m_layerColors.clear();
    m_layers.append("Default");
    m_layerVisibility["Default"] = true;
    m_layerColors["Default"] = Qt::black;
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

// Undo/Redo operations
void Document::undo()
{
    if (m_undoStack->canUndo()) {
        m_undoStack->undo();
    }
}

void Document::redo()
{
    if (m_undoStack->canRedo()) {
        m_undoStack->redo();
    }
}

bool Document::canUndo() const
{
    return m_undoStack->canUndo();
}

bool Document::canRedo() const
{
    return m_undoStack->canRedo();
}

// Direct operations (used by commands - do not use directly!)
void Document::addObjectDirect(Geometry::GeometryObject* object)
{
    if (object && !m_objects.contains(object)) {
        m_objects.append(object);

        // Connect object's changed signal
        connect(object, &Geometry::GeometryObject::changed,
                this, [this, object]() {
            emit objectChanged(object);
            notifyModified();
        });

        emit objectAdded(object);
    }
}

void Document::removeObjectDirect(Geometry::GeometryObject* object)
{
    if (object && m_objects.contains(object)) {
        m_objects.removeAll(object);
        m_selectedObjects.removeAll(object);
        emit objectRemoved(object);
    }
}

void Document::notifyObjectChanged(Geometry::GeometryObject* object)
{
    if (object) {
        emit objectChanged(object);
        notifyModified();
    }
}

} // namespace PatternCAD
