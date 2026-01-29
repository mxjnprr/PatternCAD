/**
 * Commands.cpp
 *
 * Implementation of Undo/Redo commands
 */

#include "Commands.h"
#include "Document.h"
#include "../geometry/GeometryObject.h"
#include "../geometry/Polyline.h"
#include "../geometry/Line.h"
#include "../geometry/Rectangle.h"
#include "../geometry/Circle.h"

namespace PatternCAD {

// ============================================================================
// AddObjectCommand
// ============================================================================

AddObjectCommand::AddObjectCommand(Document* document, Geometry::GeometryObject* object, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_object(object)
    , m_ownsObject(true)
{
    setText(QObject::tr("Add %1").arg(object->typeName()));
}

AddObjectCommand::~AddObjectCommand()
{
    if (m_ownsObject) {
        delete m_object;
    }
}

void AddObjectCommand::undo()
{
    m_document->removeObjectDirect(m_object);
    m_ownsObject = true;
}

void AddObjectCommand::redo()
{
    m_document->addObjectDirect(m_object);
    m_ownsObject = false;
}

// ============================================================================
// RemoveObjectCommand
// ============================================================================

RemoveObjectCommand::RemoveObjectCommand(Document* document, Geometry::GeometryObject* object, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_object(object)
    , m_ownsObject(false)
{
    setText(QObject::tr("Remove %1").arg(object->typeName()));
}

RemoveObjectCommand::~RemoveObjectCommand()
{
    if (m_ownsObject) {
        delete m_object;
    }
}

void RemoveObjectCommand::undo()
{
    m_document->addObjectDirect(m_object);
    m_ownsObject = false;
}

void RemoveObjectCommand::redo()
{
    m_document->removeObjectDirect(m_object);
    m_ownsObject = true;
}

// ============================================================================
// RemoveObjectsCommand
// ============================================================================

RemoveObjectsCommand::RemoveObjectsCommand(Document* document, const QList<Geometry::GeometryObject*>& objects, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_objects(objects)
    , m_ownsObjects(false)
{
    setText(QObject::tr("Remove %1 objects").arg(objects.size()));
}

RemoveObjectsCommand::~RemoveObjectsCommand()
{
    if (m_ownsObjects) {
        for (auto* obj : m_objects) {
            delete obj;
        }
    }
}

void RemoveObjectsCommand::undo()
{
    for (auto* obj : m_objects) {
        m_document->addObjectDirect(obj);
    }
    m_ownsObjects = false;
}

void RemoveObjectsCommand::redo()
{
    for (auto* obj : m_objects) {
        m_document->removeObjectDirect(obj);
    }
    m_ownsObjects = true;
}

// ============================================================================
// MoveObjectCommand
// ============================================================================

MoveObjectCommand::MoveObjectCommand(Geometry::GeometryObject* object, const QPointF& oldPos, const QPointF& newPos, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_oldPos(oldPos)
    , m_newPos(newPos)
{
    setText(QObject::tr("Move %1").arg(object->typeName()));
}

void MoveObjectCommand::undo()
{
    m_object->translate(m_oldPos - m_newPos);
}

void MoveObjectCommand::redo()
{
    m_object->translate(m_newPos - m_oldPos);
}

bool MoveObjectCommand::mergeWith(const QUndoCommand* other)
{
    if (other->id() != id()) {
        return false;
    }

    const MoveObjectCommand* cmd = static_cast<const MoveObjectCommand*>(other);
    if (cmd->m_object != m_object) {
        return false;
    }

    m_newPos = cmd->m_newPos;
    return true;
}

// ============================================================================
// MoveObjectsCommand
// ============================================================================

MoveObjectsCommand::MoveObjectsCommand(const QList<Geometry::GeometryObject*>& objects, const QPointF& offset, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_objects(objects)
    , m_offset(offset)
{
    setText(QObject::tr("Move %1 objects").arg(objects.size()));
}

void MoveObjectsCommand::undo()
{
    for (auto* obj : m_objects) {
        obj->translate(-m_offset);
    }
}

void MoveObjectsCommand::redo()
{
    for (auto* obj : m_objects) {
        obj->translate(m_offset);
    }
}

// ============================================================================
// ChangeLayerCommand
// ============================================================================

ChangeLayerCommand::ChangeLayerCommand(Geometry::GeometryObject* object, const QString& oldLayer, const QString& newLayer, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_oldLayer(oldLayer)
    , m_newLayer(newLayer)
{
    setText(QObject::tr("Change layer to %1").arg(newLayer));
}

void ChangeLayerCommand::undo()
{
    m_object->setLayer(m_oldLayer);
}

void ChangeLayerCommand::redo()
{
    m_object->setLayer(m_newLayer);
}

// ============================================================================
// ChangeLayersCommand
// ============================================================================

ChangeLayersCommand::ChangeLayersCommand(const QList<Geometry::GeometryObject*>& objects, const QString& newLayer, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_newLayer(newLayer)
{
    setText(QObject::tr("Change %1 objects to layer %2").arg(objects.size()).arg(newLayer));

    for (auto* obj : objects) {
        ObjectLayerPair pair;
        pair.object = obj;
        pair.oldLayer = obj->layer();
        m_objects.append(pair);
    }
}

void ChangeLayersCommand::undo()
{
    for (const auto& pair : m_objects) {
        pair.object->setLayer(pair.oldLayer);
    }
}

void ChangeLayersCommand::redo()
{
    for (const auto& pair : m_objects) {
        pair.object->setLayer(m_newLayer);
    }
}

// MoveVertexCommand implementation
MoveVertexCommand::MoveVertexCommand(Geometry::GeometryObject* object, int vertexIndex,
                                     const QPointF& oldPosition, const QPointF& newPosition,
                                     QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_vertexIndex(vertexIndex)
    , m_oldPosition(oldPosition)
    , m_newPosition(newPosition)
{
    setText(QObject::tr("Move Vertex"));
}

void MoveVertexCommand::undo()
{
    if (!m_object) return;

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_object)) {
        polyline->updateVertex(m_vertexIndex, m_oldPosition);
    } else if (auto* line = dynamic_cast<Geometry::Line*>(m_object)) {
        if (m_vertexIndex == 0) {
            line->setStart(m_oldPosition);
        } else {
            line->setEnd(m_oldPosition);
        }
    }
}

void MoveVertexCommand::redo()
{
    if (!m_object) return;

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_object)) {
        polyline->updateVertex(m_vertexIndex, m_newPosition);
    } else if (auto* line = dynamic_cast<Geometry::Line*>(m_object)) {
        if (m_vertexIndex == 0) {
            line->setStart(m_newPosition);
        } else {
            line->setEnd(m_newPosition);
        }
    }
}

// ModifyHandleCommand implementation
ModifyHandleCommand::ModifyHandleCommand(Geometry::GeometryObject* object, int vertexIndex, int side,
                                         const QPointF& oldTangent, double oldTension,
                                         const QPointF& newTangent, double newTension,
                                         QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_vertexIndex(vertexIndex)
    , m_side(side)
    , m_oldTangent(oldTangent)
    , m_oldTension(oldTension)
    , m_newTangent(newTangent)
    , m_newTension(newTension)
{
    setText(QObject::tr("Modify Curve Handle"));
}

void ModifyHandleCommand::undo()
{
    if (!m_object) return;

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_object)) {
        auto vertices = polyline->vertices();
        if (m_vertexIndex >= 0 && m_vertexIndex < vertices.size()) {
            vertices[m_vertexIndex].tangent = m_oldTangent;
            if (m_side < 0) {
                vertices[m_vertexIndex].incomingTension = m_oldTension;
            } else {
                vertices[m_vertexIndex].outgoingTension = m_oldTension;
            }
            polyline->setVertices(vertices);
        }
    }
}

void ModifyHandleCommand::redo()
{
    if (!m_object) return;

    if (auto* polyline = dynamic_cast<Geometry::Polyline*>(m_object)) {
        auto vertices = polyline->vertices();
        if (m_vertexIndex >= 0 && m_vertexIndex < vertices.size()) {
            vertices[m_vertexIndex].tangent = m_newTangent;
            if (m_side < 0) {
                vertices[m_vertexIndex].incomingTension = m_newTension;
            } else {
                vertices[m_vertexIndex].outgoingTension = m_newTension;
            }
            polyline->setVertices(vertices);
        }
    }
}

// ============================================================================
// UpdatePropertyCommand
// ============================================================================

UpdatePropertyCommand::UpdatePropertyCommand(const QList<Geometry::GeometryObject*>& objects,
                                            const QString& propertyName,
                                            const QVariant& newValue,
                                            QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_propertyName(propertyName)
    , m_newValue(newValue)
{
    // Store old values for all objects
    for (auto* obj : objects) {
        if (obj) {
            ObjectProperty prop;
            prop.object = obj;
            prop.oldValue = getProperty(obj, propertyName);
            m_objects.append(prop);
        }
    }

    if (objects.size() == 1) {
        setText(QObject::tr("Update %1").arg(propertyName));
    } else {
        setText(QObject::tr("Update %1 for %2 objects").arg(propertyName).arg(objects.size()));
    }
}

void UpdatePropertyCommand::undo()
{
    for (const auto& prop : m_objects) {
        if (prop.object) {
            setProperty(prop.object, m_propertyName, prop.oldValue);
        }
    }
}

void UpdatePropertyCommand::redo()
{
    for (const auto& prop : m_objects) {
        if (prop.object) {
            setProperty(prop.object, m_propertyName, m_newValue);
        }
    }
}

QVariant UpdatePropertyCommand::getProperty(Geometry::GeometryObject* object, const QString& propertyName) const
{
    if (propertyName == "name") {
        return object->name();
    } else if (propertyName == "layer") {
        return object->layer();
    } else if (propertyName == "lineWeight") {
        return object->lineWeight();
    } else if (propertyName == "lineColor") {
        return object->lineColor();
    } else if (propertyName == "lineStyle") {
        return static_cast<int>(object->lineStyle());
    } else if (propertyName == "visible") {
        return object->isVisible();
    } else if (propertyName == "locked") {
        return object->isLocked();
    }

    // Type-specific properties
    if (auto* rect = dynamic_cast<Geometry::Rectangle*>(object)) {
        if (propertyName == "x") {
            return rect->topLeft().x();
        } else if (propertyName == "y") {
            return rect->topLeft().y();
        } else if (propertyName == "width") {
            return rect->width();
        } else if (propertyName == "height") {
            return rect->height();
        }
    } else if (auto* line = dynamic_cast<Geometry::Line*>(object)) {
        if (propertyName == "x") {
            return line->start().x();
        } else if (propertyName == "y") {
            return line->start().y();
        }
    } else if (auto* circle = dynamic_cast<Geometry::Circle*>(object)) {
        if (propertyName == "x") {
            return circle->center().x();
        } else if (propertyName == "y") {
            return circle->center().y();
        } else if (propertyName == "radius") {
            return circle->radius();
        }
    } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(object)) {
        auto vertices = polyline->vertices();
        if (!vertices.isEmpty()) {
            if (propertyName == "x") {
                return vertices.first().position.x();
            } else if (propertyName == "y") {
                return vertices.first().position.y();
            }
        }
    }

    return QVariant();
}

void UpdatePropertyCommand::setProperty(Geometry::GeometryObject* object, const QString& propertyName, const QVariant& value)
{
    if (propertyName == "name") {
        object->setName(value.toString());
    } else if (propertyName == "layer") {
        object->setLayer(value.toString());
    } else if (propertyName == "lineWeight") {
        object->setLineWeight(value.toDouble());
    } else if (propertyName == "lineColor") {
        object->setLineColor(value.value<QColor>());
    } else if (propertyName == "lineStyle") {
        object->setLineStyle(static_cast<Geometry::GeometryObject::LineStyle>(value.toInt()));
    } else if (propertyName == "visible") {
        object->setVisible(value.toBool());
    } else if (propertyName == "locked") {
        object->setLocked(value.toBool());
    }

    // Type-specific properties
    if (auto* rect = dynamic_cast<Geometry::Rectangle*>(object)) {
        if (propertyName == "x") {
            QPointF tl = rect->topLeft();
            tl.setX(value.toDouble());
            rect->setTopLeft(tl);
        } else if (propertyName == "y") {
            QPointF tl = rect->topLeft();
            tl.setY(value.toDouble());
            rect->setTopLeft(tl);
        } else if (propertyName == "width") {
            rect->setWidth(value.toDouble());
        } else if (propertyName == "height") {
            rect->setHeight(value.toDouble());
        }
    } else if (auto* line = dynamic_cast<Geometry::Line*>(object)) {
        if (propertyName == "x") {
            QPointF start = line->start();
            QPointF end = line->end();
            double dx = value.toDouble() - start.x();
            line->setStart(QPointF(value.toDouble(), start.y()));
            line->setEnd(QPointF(end.x() + dx, end.y()));
        } else if (propertyName == "y") {
            QPointF start = line->start();
            QPointF end = line->end();
            double dy = value.toDouble() - start.y();
            line->setStart(QPointF(start.x(), value.toDouble()));
            line->setEnd(QPointF(end.x(), end.y() + dy));
        }
    } else if (auto* circle = dynamic_cast<Geometry::Circle*>(object)) {
        if (propertyName == "x") {
            QPointF center = circle->center();
            center.setX(value.toDouble());
            circle->setCenter(center);
        } else if (propertyName == "y") {
            QPointF center = circle->center();
            center.setY(value.toDouble());
            circle->setCenter(center);
        } else if (propertyName == "radius") {
            circle->setRadius(value.toDouble());
        }
    } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(object)) {
        auto vertices = polyline->vertices();
        if (!vertices.isEmpty()) {
            if (propertyName == "x") {
                double dx = value.toDouble() - vertices.first().position.x();
                for (auto& v : vertices) {
                    v.position.setX(v.position.x() + dx);
                }
                polyline->setVertices(vertices);
            } else if (propertyName == "y") {
                double dy = value.toDouble() - vertices.first().position.y();
                for (auto& v : vertices) {
                    v.position.setY(v.position.y() + dy);
                }
                polyline->setVertices(vertices);
            }
        }
    }
}

// RotateObjectsCommand
RotateObjectsCommand::RotateObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                                           double angleDegrees,
                                           const QPointF& center,
                                           QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_objects(objects)
    , m_angleDegrees(angleDegrees)
    , m_center(center)
{
    setText(QObject::tr("Rotate %n object(s)", "", objects.size()));
}

void RotateObjectsCommand::undo()
{
    // Undo rotation by rotating back with negative angle
    for (auto* obj : m_objects) {
        if (obj) {
            obj->rotate(-m_angleDegrees, m_center);
        }
    }
}

void RotateObjectsCommand::redo()
{
    // Apply rotation
    for (auto* obj : m_objects) {
        if (obj) {
            obj->rotate(m_angleDegrees, m_center);
        }
    }
}

} // namespace PatternCAD
