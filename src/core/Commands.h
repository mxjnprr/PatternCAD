/**
 * Commands.h
 *
 * Undo/Redo command classes for PatternCAD
 */

#ifndef PATTERNCAD_COMMANDS_H
#define PATTERNCAD_COMMANDS_H

#include <QUndoCommand>
#include <QString>
#include <QPointF>
#include <QList>
#include <QVariant>
#include <QColor>

namespace PatternCAD {

class Document;

namespace Geometry {
    class GeometryObject;
}

/**
 * AddObjectCommand - Command to add an object to the document
 */
class AddObjectCommand : public QUndoCommand
{
public:
    AddObjectCommand(Document* document, Geometry::GeometryObject* object, QUndoCommand* parent = nullptr);
    ~AddObjectCommand();

    void undo() override;
    void redo() override;

private:
    Document* m_document;
    Geometry::GeometryObject* m_object;
    bool m_ownsObject;
};

/**
 * RemoveObjectCommand - Command to remove an object from the document
 */
class RemoveObjectCommand : public QUndoCommand
{
public:
    RemoveObjectCommand(Document* document, Geometry::GeometryObject* object, QUndoCommand* parent = nullptr);
    ~RemoveObjectCommand();

    void undo() override;
    void redo() override;

private:
    Document* m_document;
    Geometry::GeometryObject* m_object;
    bool m_ownsObject;
};

/**
 * RemoveObjectsCommand - Command to remove multiple objects from the document
 */
class RemoveObjectsCommand : public QUndoCommand
{
public:
    RemoveObjectsCommand(Document* document, const QList<Geometry::GeometryObject*>& objects, QUndoCommand* parent = nullptr);
    ~RemoveObjectsCommand();

    void undo() override;
    void redo() override;

private:
    Document* m_document;
    QList<Geometry::GeometryObject*> m_objects;
    bool m_ownsObjects;
};

/**
 * MoveObjectCommand - Command to move an object
 */
class MoveObjectCommand : public QUndoCommand
{
public:
    MoveObjectCommand(Geometry::GeometryObject* object, const QPointF& oldPos, const QPointF& newPos, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override { return 1; }
    bool mergeWith(const QUndoCommand* other) override;

private:
    Geometry::GeometryObject* m_object;
    QPointF m_oldPos;
    QPointF m_newPos;
};

/**
 * MoveObjectsCommand - Command to move multiple objects
 */
class MoveObjectsCommand : public QUndoCommand
{
public:
    MoveObjectsCommand(const QList<Geometry::GeometryObject*>& objects, const QPointF& offset, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QList<Geometry::GeometryObject*> m_objects;
    QPointF m_offset;
};

/**
 * ChangeLayerCommand - Command to change an object's layer
 */
class ChangeLayerCommand : public QUndoCommand
{
public:
    ChangeLayerCommand(Geometry::GeometryObject* object, const QString& oldLayer, const QString& newLayer, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Geometry::GeometryObject* m_object;
    QString m_oldLayer;
    QString m_newLayer;
};

/**
 * ChangeLayersCommand - Command to change multiple objects' layers
 */
class ChangeLayersCommand : public QUndoCommand
{
public:
    ChangeLayersCommand(const QList<Geometry::GeometryObject*>& objects, const QString& newLayer, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    struct ObjectLayerPair {
        Geometry::GeometryObject* object;
        QString oldLayer;
    };
    QList<ObjectLayerPair> m_objects;
    QString m_newLayer;
};

/**
 * MoveVertexCommand - Command to move a vertex of a polyline
 */
class MoveVertexCommand : public QUndoCommand
{
public:
    MoveVertexCommand(Geometry::GeometryObject* object, int vertexIndex,
                     const QPointF& oldPosition, const QPointF& newPosition,
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Geometry::GeometryObject* m_object;
    int m_vertexIndex;
    QPointF m_oldPosition;
    QPointF m_newPosition;
};

/**
 * ModifyHandleCommand - Command to modify a curve handle's tangent and tension
 */
class ModifyHandleCommand : public QUndoCommand
{
public:
    ModifyHandleCommand(Geometry::GeometryObject* object, int vertexIndex, int side,
                       const QPointF& oldTangent, double oldTension,
                       const QPointF& newTangent, double newTension,
                       QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Geometry::GeometryObject* m_object;
    int m_vertexIndex;
    int m_side;  // -1 = incoming, +1 = outgoing
    QPointF m_oldTangent;
    double m_oldTension;
    QPointF m_newTangent;
    double m_newTension;
};

/**
 * UpdatePropertyCommand - Command to update a single property of an object or multiple objects
 */
class UpdatePropertyCommand : public QUndoCommand
{
public:
    UpdatePropertyCommand(const QList<Geometry::GeometryObject*>& objects,
                         const QString& propertyName,
                         const QVariant& newValue,
                         QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    struct ObjectProperty {
        Geometry::GeometryObject* object;
        QVariant oldValue;
    };

    QList<ObjectProperty> m_objects;
    QString m_propertyName;
    QVariant m_newValue;

    // Helper methods
    QVariant getProperty(Geometry::GeometryObject* object, const QString& propertyName) const;
    void setProperty(Geometry::GeometryObject* object, const QString& propertyName, const QVariant& value);
};

} // namespace PatternCAD

#endif // PATTERNCAD_COMMANDS_H
