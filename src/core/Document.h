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
    QList<Geometry::GeometryObject*> objects() const;
    QList<Geometry::GeometryObject*> objectsOnLayer(const QString& layerName) const;

    // Selection
    QList<Geometry::GeometryObject*> selectedObjects() const;
    void setSelectedObjects(const QList<Geometry::GeometryObject*>& objects);
    void clearSelection();
    void selectAll();

    // Layers
    QStringList layers() const;
    void addLayer(const QString& layerName);
    void removeLayer(const QString& layerName);
    void renameLayer(const QString& oldName, const QString& newName);
    QString activeLayer() const;
    void setActiveLayer(const QString& layerName);

    // Undo/Redo (to be implemented)
    // void undo();
    // void redo();
    // bool canUndo() const;
    // bool canRedo() const;

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

private:
    // Private members
    QString m_name;
    bool m_modified;
    QList<Geometry::GeometryObject*> m_objects;
    QList<Geometry::GeometryObject*> m_selectedObjects;
    QStringList m_layers;
    QString m_activeLayer;

    // Helper methods
    void notifyModified();
};

} // namespace PatternCAD

#endif // PATTERNCAD_DOCUMENT_H
