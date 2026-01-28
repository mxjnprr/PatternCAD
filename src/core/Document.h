/**
 * Document.h
 *
 * Document class representing a single pattern document
 */

#ifndef PATTERNCAD_DOCUMENT_H
#define PATTERNCAD_DOCUMENT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QColor>
#include <QUndoStack>
#include <memory>

namespace PatternCAD {

// Forward declarations
namespace Geometry {
    class GeometryObject;
}

/**
 * Document represents a single pattern document containing:
 * - Geometric objects (lines, circles, curves, etc.)
 * - Layers for organization
 * - Selection state
 * - Undo/redo history
 * - Document properties
 */
class Document : public QObject
{
    Q_OBJECT

public:
    explicit Document(QObject* parent = nullptr);
    ~Document();

    // Document properties
    QString name() const;
    void setName(const QString& name);

    bool isModified() const;
    void setModified(bool modified);

    // Objects management
    void addObject(Geometry::GeometryObject* object);
    void removeObject(Geometry::GeometryObject* object);
    void removeObjects(const QList<Geometry::GeometryObject*>& objects);
    QList<Geometry::GeometryObject*> objects() const;
    QList<Geometry::GeometryObject*> objectsOnLayer(const QString& layerName) const;

    // Selection
    QList<Geometry::GeometryObject*> selectedObjects() const;
    void setSelectedObjects(const QList<Geometry::GeometryObject*>& objects);
    void clearSelection();
    void selectAll();

    // Layers
    QStringList layers() const;
    void addLayer(const QString& layerName, const QColor& color = Qt::black);
    void removeLayer(const QString& layerName);
    void renameLayer(const QString& oldName, const QString& newName);
    QString activeLayer() const;
    void setActiveLayer(const QString& layerName);
    bool isLayerVisible(const QString& layerName) const;
    void setLayerVisible(const QString& layerName, bool visible);
    QColor layerColor(const QString& layerName) const;
    void setLayerColor(const QString& layerName, const QColor& color);
    bool isLayerLocked(const QString& layerName) const;
    void setLayerLocked(const QString& layerName, bool locked);

    // Undo/Redo
    QUndoStack* undoStack() const { return m_undoStack; }
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;

    // Direct object operations (used by commands - do not use directly)
    void addObjectDirect(Geometry::GeometryObject* object);
    void removeObjectDirect(Geometry::GeometryObject* object);

    // Notify that an object has changed (for external modifications)
    void notifyObjectChanged(Geometry::GeometryObject* object);

    // File operations
    bool save(const QString& filepath);
    bool load(const QString& filepath);
    void clear();

signals:
    void nameChanged(const QString& name);
    void modifiedChanged(bool modified);
    void objectAdded(Geometry::GeometryObject* object);
    void objectRemoved(Geometry::GeometryObject* object);
    void objectChanged(Geometry::GeometryObject* object);
    void selectionChanged();
    void layerAdded(const QString& layerName);
    void layerRemoved(const QString& layerName);
    void layerRenamed(const QString& oldName, const QString& newName);
    void activeLayerChanged(const QString& layerName);
    void layerVisibilityChanged(const QString& layerName, bool visible);

private:
    // Private members
    QString m_name;
    bool m_modified;
    QList<Geometry::GeometryObject*> m_objects;
    QList<Geometry::GeometryObject*> m_selectedObjects;
    QStringList m_layers;
    QMap<QString, bool> m_layerVisibility;
    QMap<QString, bool> m_layerLocked;
    QMap<QString, QColor> m_layerColors;
    QString m_activeLayer;
    QUndoStack* m_undoStack;

    // Helper methods
    void notifyModified();
};

} // namespace PatternCAD

#endif // PATTERNCAD_DOCUMENT_H
