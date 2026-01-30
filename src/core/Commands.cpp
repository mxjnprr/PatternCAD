/**
 * Commands.cpp
 *
 * Implementation of Undo/Redo commands
 */

#include "Commands.h"
#include "Document.h"
#include "../geometry/GeometryObject.h"
#include "../geometry/Point2D.h"
#include "../geometry/Line.h"
#include "../geometry/Circle.h"
#include "../geometry/Rectangle.h"
#include "../geometry/Polyline.h"
#include "../geometry/CubicBezier.h"
#include "../geometry/Notch.h"
#include "../geometry/MatchPoint.h"
#include <QDebug>

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

// ============================================================================
// MirrorObjectsCommand
// ============================================================================

namespace {
    // Helper function to clone a geometry object
    Geometry::GeometryObject* cloneObject(Geometry::GeometryObject* obj) {
        if (!obj) return nullptr;

        using namespace Geometry;

        switch (obj->type()) {
            case ObjectType::Point: {
                auto* point = qobject_cast<Point2D*>(obj);
                if (point) {
                    auto* clone = new Point2D(point->position());
                    clone->setName(point->name());
                    clone->setLayer(point->layer());
                    clone->setLineWeight(point->lineWeight());
                    clone->setLineColor(point->lineColor());
                    clone->setLineStyle(point->lineStyle());
                    return clone;
                }
                break;
            }
            case ObjectType::Line: {
                auto* line = qobject_cast<Line*>(obj);
                if (line) {
                    auto* clone = new Line(line->start(), line->end());
                    clone->setName(line->name());
                    clone->setLayer(line->layer());
                    clone->setLineWeight(line->lineWeight());
                    clone->setLineColor(line->lineColor());
                    clone->setLineStyle(line->lineStyle());
                    return clone;
                }
                break;
            }
            case ObjectType::Circle: {
                auto* circle = qobject_cast<Circle*>(obj);
                if (circle) {
                    auto* clone = new Circle(circle->center(), circle->radius());
                    clone->setName(circle->name());
                    clone->setLayer(circle->layer());
                    clone->setLineWeight(circle->lineWeight());
                    clone->setLineColor(circle->lineColor());
                    clone->setLineStyle(circle->lineStyle());
                    return clone;
                }
                break;
            }
            case ObjectType::Rectangle: {
                auto* rect = qobject_cast<Rectangle*>(obj);
                if (rect) {
                    auto* clone = new Rectangle(rect->topLeft(), rect->width(), rect->height());
                    clone->setName(rect->name());
                    clone->setLayer(rect->layer());
                    clone->setLineWeight(rect->lineWeight());
                    clone->setLineColor(rect->lineColor());
                    clone->setLineStyle(rect->lineStyle());
                    return clone;
                }
                break;
            }
            case ObjectType::Polyline: {
                auto* poly = qobject_cast<Polyline*>(obj);
                if (poly) {
                    auto* clone = new Polyline();
                    clone->setVertices(poly->vertices());
                    clone->setClosed(poly->isClosed());
                    clone->setName(poly->name());
                    clone->setLayer(poly->layer());
                    clone->setLineWeight(poly->lineWeight());
                    clone->setLineColor(poly->lineColor());
                    clone->setLineStyle(poly->lineStyle());
                    return clone;
                }
                break;
            }
            case ObjectType::CubicBezier: {
                auto* bezier = qobject_cast<CubicBezier*>(obj);
                if (bezier) {
                    auto* clone = new CubicBezier();
                    clone->setPoints(bezier->p0(), bezier->p1(), bezier->p2(), bezier->p3());
                    clone->setName(bezier->name());
                    clone->setLayer(bezier->layer());
                    clone->setLineWeight(bezier->lineWeight());
                    clone->setLineColor(bezier->lineColor());
                    clone->setLineStyle(bezier->lineStyle());
                    return clone;
                }
                break;
            }
            default:
                break;
        }

        return nullptr;
    }
}

MirrorObjectsCommand::MirrorObjectsCommand(Document* document,
                                         const QList<Geometry::GeometryObject*>& objects,
                                         const QPointF& axisPoint1,
                                         const QPointF& axisPoint2,
                                         QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_originalObjects(objects)
    , m_axisPoint1(axisPoint1)
    , m_axisPoint2(axisPoint2)
    , m_firstRedo(true)
{
    setText(QObject::tr("Mirror %n object(s)", "", objects.size()));
}

MirrorObjectsCommand::~MirrorObjectsCommand()
{
    // Clean up mirrored objects if we own them (they're not in the document)
    // We own them when the command is undone
    if (!m_mirroredObjects.isEmpty()) {
        // Check if first object is in document
        bool inDocument = false;
        if (!m_mirroredObjects.isEmpty() && m_document) {
            inDocument = m_document->objects().contains(m_mirroredObjects.first());
        }
        if (!inDocument) {
            qDeleteAll(m_mirroredObjects);
        }
    }
}

void MirrorObjectsCommand::undo()
{
    // Remove mirrored objects from document
    for (auto* obj : m_mirroredObjects) {
        if (obj) {
            m_document->removeObjectDirect(obj);
        }
    }
}

void MirrorObjectsCommand::redo()
{
    if (m_firstRedo) {
        // First execution: create the mirrored copies
        m_mirroredObjects.clear();
        for (auto* original : m_originalObjects) {
            if (original) {
                // Clone the object
                auto* mirrored = cloneObject(original);
                if (mirrored) {
                    // Mirror the clone
                    mirrored->mirror(m_axisPoint1, m_axisPoint2);
                    // Add to document
                    m_document->addObjectDirect(mirrored);
                    // Store the mirrored object
                    m_mirroredObjects.append(mirrored);
                }
            }
        }
        m_firstRedo = false;
    } else {
        // Subsequent executions: re-add existing mirrored objects
        for (auto* obj : m_mirroredObjects) {
            if (obj) {
                m_document->addObjectDirect(obj);
            }
        }
    }
}

// ============================================================================
// ScaleObjectsCommand
// ============================================================================

ScaleObjectsCommand::ScaleObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                                       double scaleX,
                                       double scaleY,
                                       const QPointF& origin,
                                       QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_objects(objects)
    , m_scaleX(scaleX)
    , m_scaleY(scaleY)
    , m_origin(origin)
{
    setText(QObject::tr("Scale %n object(s)", "", objects.size()));
}

void ScaleObjectsCommand::undo()
{
    // Undo scale by scaling with inverse factors
    for (auto* obj : m_objects) {
        if (obj) {
            obj->scale(1.0 / m_scaleX, 1.0 / m_scaleY, m_origin);
        }
    }
}

void ScaleObjectsCommand::redo()
{
    qDebug() << "ScaleObjectsCommand::redo - scaleX=" << m_scaleX << "scaleY=" << m_scaleY << "origin=" << m_origin;
    // Apply scale
    for (auto* obj : m_objects) {
        if (obj) {
            obj->scale(m_scaleX, m_scaleY, m_origin);
        }
    }
}

// ============================================================================
// AlignObjectsCommand
// ============================================================================

AlignObjectsCommand::AlignObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                                       AlignMode mode,
                                       QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_mode(mode)
{
    // Calculate reference bounds (combined bounds of all objects)
    if (objects.isEmpty()) {
        setText(QObject::tr("Align"));
        return;
    }

    QRectF referenceBounds = objects.first()->boundingRect();
    for (int i = 1; i < objects.size(); ++i) {
        referenceBounds = referenceBounds.united(objects[i]->boundingRect());
    }

    // Calculate offset for each object
    for (auto* obj : objects) {
        if (!obj) continue;

        QRectF objBounds = obj->boundingRect();
        QPointF offset(0, 0);

        switch (mode) {
            case AlignMode::Left:
                offset.setX(referenceBounds.left() - objBounds.left());
                break;
            case AlignMode::Right:
                offset.setX(referenceBounds.right() - objBounds.right());
                break;
            case AlignMode::Top:
                offset.setY(referenceBounds.top() - objBounds.top());
                break;
            case AlignMode::Bottom:
                offset.setY(referenceBounds.bottom() - objBounds.bottom());
                break;
            case AlignMode::CenterHorizontal:
                offset.setX(referenceBounds.center().x() - objBounds.center().x());
                break;
            case AlignMode::CenterVertical:
                offset.setY(referenceBounds.center().y() - objBounds.center().y());
                break;
        }

        ObjectOffset objOffset;
        objOffset.object = obj;
        objOffset.originalOffset = offset;
        m_objects.append(objOffset);
    }

    // Set command text based on mode
    QString modeText;
    switch (mode) {
        case AlignMode::Left: modeText = "Left"; break;
        case AlignMode::Right: modeText = "Right"; break;
        case AlignMode::Top: modeText = "Top"; break;
        case AlignMode::Bottom: modeText = "Bottom"; break;
        case AlignMode::CenterHorizontal: modeText = "Center Horizontal"; break;
        case AlignMode::CenterVertical: modeText = "Center Vertical"; break;
    }
    setText(QObject::tr("Align %1").arg(modeText));
}

void AlignObjectsCommand::undo()
{
    // Move objects back by negating their offsets
    for (const auto& objOffset : m_objects) {
        if (objOffset.object) {
            objOffset.object->translate(-objOffset.originalOffset);
        }
    }
}

void AlignObjectsCommand::redo()
{
    // Apply alignment offsets
    for (const auto& objOffset : m_objects) {
        if (objOffset.object) {
            objOffset.object->translate(objOffset.originalOffset);
        }
    }
}

// ============================================================================
// DistributeObjectsCommand
// ============================================================================

DistributeObjectsCommand::DistributeObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                                                 DistributeMode mode,
                                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_mode(mode)
{
    if (objects.size() < 3) {
        setText(QObject::tr("Distribute"));
        return;
    }

    // Create a sorted copy of objects
    QList<Geometry::GeometryObject*> sortedObjects = objects;

    // Sort objects by position (center X for horizontal, center Y for vertical)
    if (mode == DistributeMode::Horizontal) {
        std::sort(sortedObjects.begin(), sortedObjects.end(),
                 [](Geometry::GeometryObject* a, Geometry::GeometryObject* b) {
                     return a->boundingRect().center().x() < b->boundingRect().center().x();
                 });
    } else {
        std::sort(sortedObjects.begin(), sortedObjects.end(),
                 [](Geometry::GeometryObject* a, Geometry::GeometryObject* b) {
                     return a->boundingRect().center().y() < b->boundingRect().center().y();
                 });
    }

    // Keep first and last objects fixed
    Geometry::GeometryObject* firstObj = sortedObjects.first();
    Geometry::GeometryObject* lastObj = sortedObjects.last();

    // Calculate total span between first and last object centers
    double totalSpan;
    if (mode == DistributeMode::Horizontal) {
        totalSpan = lastObj->boundingRect().center().x() - firstObj->boundingRect().center().x();
    } else {
        totalSpan = lastObj->boundingRect().center().y() - firstObj->boundingRect().center().y();
    }

    // Calculate equal gap size between object centers
    double gapSize = totalSpan / (sortedObjects.size() - 1);

    // Calculate offsets for middle objects (first and last stay fixed)
    for (int i = 0; i < sortedObjects.size(); ++i) {
        auto* obj = sortedObjects[i];
        if (!obj) continue;

        QPointF currentCenter = obj->boundingRect().center();
        QPointF targetCenter;

        if (mode == DistributeMode::Horizontal) {
            double targetX = firstObj->boundingRect().center().x() + i * gapSize;
            targetCenter = QPointF(targetX, currentCenter.y());
        } else {
            double targetY = firstObj->boundingRect().center().y() + i * gapSize;
            targetCenter = QPointF(currentCenter.x(), targetY);
        }

        QPointF offset = targetCenter - currentCenter;

        ObjectOffset objOffset;
        objOffset.object = obj;
        objOffset.originalOffset = offset;
        m_objects.append(objOffset);
    }

    // Set command text
    QString modeText = (mode == DistributeMode::Horizontal) ? "Horizontal" : "Vertical";
    setText(QObject::tr("Distribute %1").arg(modeText));
}

void DistributeObjectsCommand::undo()
{
    // Move objects back by negating their offsets
    for (const auto& objOffset : m_objects) {
        if (objOffset.object) {
            objOffset.object->translate(-objOffset.originalOffset);
        }
    }
}

void DistributeObjectsCommand::redo()
{
    // Apply distribution offsets
    for (const auto& objOffset : m_objects) {
        if (objOffset.object) {
            objOffset.object->translate(objOffset.originalOffset);
        }
    }
}

// ============================================================================
// DeleteVertexCommand
// ============================================================================

DeleteVertexCommand::DeleteVertexCommand(Geometry::GeometryObject* object, int vertexIndex,
                                       QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_vertexIndex(vertexIndex)
{
    // Store vertex data for undo
    if (auto* polyline = qobject_cast<Geometry::Polyline*>(object)) {
        if (vertexIndex >= 0 && vertexIndex < polyline->vertexCount()) {
            auto vertex = polyline->vertexAt(vertexIndex);
            m_position = vertex.position;
            m_vertexType = static_cast<int>(vertex.type);
            m_incomingTension = vertex.incomingTension;
            m_outgoingTension = vertex.outgoingTension;
            m_tangent = vertex.tangent;
        }
    }
    setText(QObject::tr("Delete Vertex %1").arg(vertexIndex + 1));
}

void DeleteVertexCommand::undo()
{
    // Re-insert the vertex
    if (auto* polyline = qobject_cast<Geometry::Polyline*>(m_object)) {
        Geometry::PolylineVertex vertex(
            m_position,
            static_cast<Geometry::VertexType>(m_vertexType),
            m_incomingTension,
            m_outgoingTension,
            m_tangent
        );
        polyline->insertVertex(m_vertexIndex, vertex);
    }
}

void DeleteVertexCommand::redo()
{
    // Delete the vertex
    if (auto* polyline = qobject_cast<Geometry::Polyline*>(m_object)) {
        polyline->removeVertex(m_vertexIndex);
    }
}

// ============================================================================
// ChangeVertexTypeCommand
// ============================================================================

ChangeVertexTypeCommand::ChangeVertexTypeCommand(Geometry::GeometryObject* object, int vertexIndex,
                                                 int oldType, int newType,
                                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_vertexIndex(vertexIndex)
    , m_oldType(oldType)
    , m_newType(newType)
{
    QString oldTypeName = (oldType == static_cast<int>(Geometry::VertexType::Sharp)) ? "Sharp" : "Smooth";
    QString newTypeName = (newType == static_cast<int>(Geometry::VertexType::Sharp)) ? "Sharp" : "Smooth";
    setText(QObject::tr("Change Vertex %1: %2 → %3").arg(vertexIndex + 1).arg(oldTypeName).arg(newTypeName));
}

void ChangeVertexTypeCommand::undo()
{
    if (auto* polyline = qobject_cast<Geometry::Polyline*>(m_object)) {
        polyline->setVertexType(m_vertexIndex, static_cast<Geometry::VertexType>(m_oldType));
    }
}

void ChangeVertexTypeCommand::redo()
{
    if (auto* polyline = qobject_cast<Geometry::Polyline*>(m_object)) {
        polyline->setVertexType(m_vertexIndex, static_cast<Geometry::VertexType>(m_newType));
    }
}

// ============================================================================
// Story 004-02: Notch Commands
// ============================================================================

AddNotchCommand::AddNotchCommand(Geometry::Polyline* polyline, Notch* notch,
                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_polyline(polyline)
    , m_notch(notch)
    , m_ownsNotch(true)
{
    setText(QObject::tr("Add Notch"));
}

AddNotchCommand::~AddNotchCommand()
{
    if (m_ownsNotch) {
        delete m_notch;
    }
}

void AddNotchCommand::undo()
{
    m_polyline->removeNotch(m_notch);
    m_ownsNotch = true;
}

void AddNotchCommand::redo()
{
    m_polyline->addNotch(m_notch);
    m_ownsNotch = false;
}

RemoveNotchCommand::RemoveNotchCommand(Geometry::Polyline* polyline, Notch* notch,
                                       QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_polyline(polyline)
    , m_notch(notch)
    , m_ownsNotch(false)
{
    setText(QObject::tr("Remove Notch"));
}

RemoveNotchCommand::~RemoveNotchCommand()
{
    if (m_ownsNotch) {
        delete m_notch;
    }
}

void RemoveNotchCommand::undo()
{
    m_polyline->addNotch(m_notch);
    m_ownsNotch = false;
}

void RemoveNotchCommand::redo()
{
    m_polyline->removeNotch(m_notch);
    m_ownsNotch = true;
}

ModifyNotchCommand::ModifyNotchCommand(Notch* notch, int newStyle, double newDepth,
                                       int newSegmentIndex, double newPosition,
                                       QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_notch(notch)
    , m_newStyle(newStyle)
    , m_newDepth(newDepth)
    , m_newSegmentIndex(newSegmentIndex)
    , m_newPosition(newPosition)
{
    // Store old values
    m_oldStyle = static_cast<int>(notch->style());
    m_oldDepth = notch->depth();
    m_oldSegmentIndex = notch->segmentIndex();
    m_oldPosition = notch->position();
    
    setText(QObject::tr("Modify Notch"));
}

void ModifyNotchCommand::undo()
{
    m_notch->setStyle(static_cast<NotchStyle>(m_oldStyle));
    m_notch->setDepth(m_oldDepth);
    m_notch->setSegmentIndex(m_oldSegmentIndex);
    m_notch->setPosition(m_oldPosition);
}

void ModifyNotchCommand::redo()
{
    m_notch->setStyle(static_cast<NotchStyle>(m_newStyle));
    m_notch->setDepth(m_newDepth);
    m_notch->setSegmentIndex(m_newSegmentIndex);
    m_notch->setPosition(m_newPosition);
}

// ============================================================================
// Story 004-03: MatchPoint Commands
// ============================================================================

AddMatchPointCommand::AddMatchPointCommand(Geometry::Polyline* polyline, MatchPoint* matchPoint,
                                           QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_polyline(polyline)
    , m_matchPoint(matchPoint)
    , m_ownsMatchPoint(true)
{
    setText(QObject::tr("Add Match Point '%1'").arg(matchPoint->label()));
}

AddMatchPointCommand::~AddMatchPointCommand()
{
    if (m_ownsMatchPoint) {
        delete m_matchPoint;
    }
}

void AddMatchPointCommand::undo()
{
    m_polyline->removeMatchPoint(m_matchPoint);
    m_ownsMatchPoint = true;
}

void AddMatchPointCommand::redo()
{
    m_polyline->addMatchPoint(m_matchPoint);
    m_ownsMatchPoint = false;
}

RemoveMatchPointCommand::RemoveMatchPointCommand(Geometry::Polyline* polyline, MatchPoint* matchPoint,
                                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_polyline(polyline)
    , m_matchPoint(matchPoint)
    , m_ownsMatchPoint(false)
{
    setText(QObject::tr("Remove Match Point '%1'").arg(matchPoint->label()));
    
    // Backup linked points for restoration on undo
    m_linkedPointsBackup = matchPoint->linkedPoints();
}

RemoveMatchPointCommand::~RemoveMatchPointCommand()
{
    if (m_ownsMatchPoint) {
        delete m_matchPoint;
    }
}

void RemoveMatchPointCommand::undo()
{
    m_polyline->addMatchPoint(m_matchPoint);
    
    // Restore links
    for (MatchPoint* other : m_linkedPointsBackup) {
        m_matchPoint->linkTo(other);
    }
    
    m_ownsMatchPoint = false;
}

void RemoveMatchPointCommand::redo()
{
    // Links will be automatically broken by unlinkAll when removed
    m_polyline->removeMatchPoint(m_matchPoint);
    m_ownsMatchPoint = true;
}

ModifyMatchPointCommand::ModifyMatchPointCommand(MatchPoint* matchPoint, const QString& newLabel,
                                                 int newSegmentIndex, double newSegmentPosition,
                                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_matchPoint(matchPoint)
    , m_newLabel(newLabel)
    , m_newSegmentIndex(newSegmentIndex)
    , m_newSegmentPosition(newSegmentPosition)
{
    // Store old values
    m_oldLabel = matchPoint->label();
    m_oldSegmentIndex = matchPoint->segmentIndex();
    m_oldSegmentPosition = matchPoint->segmentPosition();
    
    setText(QObject::tr("Modify Match Point"));
}

void ModifyMatchPointCommand::undo()
{
    m_matchPoint->setLabel(m_oldLabel);
    m_matchPoint->setSegmentIndex(m_oldSegmentIndex);
    m_matchPoint->setSegmentPosition(m_oldSegmentPosition);
}

void ModifyMatchPointCommand::redo()
{
    m_matchPoint->setLabel(m_newLabel);
    m_matchPoint->setSegmentIndex(m_newSegmentIndex);
    m_matchPoint->setSegmentPosition(m_newSegmentPosition);
}

LinkMatchPointsCommand::LinkMatchPointsCommand(MatchPoint* pointA, MatchPoint* pointB, bool link,
                                               QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_pointA(pointA)
    , m_pointB(pointB)
    , m_link(link)
{
    if (link) {
        setText(QObject::tr("Link Match Points '%1' ↔ '%2'").arg(pointA->label()).arg(pointB->label()));
    } else {
        setText(QObject::tr("Unlink Match Points '%1' ↔ '%2'").arg(pointA->label()).arg(pointB->label()));
    }
}

void LinkMatchPointsCommand::undo()
{
    if (m_link) {
        // Was linking, so unlink
        m_pointA->unlinkFrom(m_pointB);
    } else {
        // Was unlinking, so re-link
        m_pointA->linkTo(m_pointB);
    }
}

void LinkMatchPointsCommand::redo()
{
    if (m_link) {
        m_pointA->linkTo(m_pointB);
    } else {
        m_pointA->unlinkFrom(m_pointB);
    }
}

// ============================================================================
// Story 004-07: Duplicate Pattern Command
// ============================================================================

DuplicatePolylineCommand::DuplicatePolylineCommand(Document* document, Geometry::Polyline* original,
                                                   QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_original(original)
    , m_duplicate(nullptr)
    , m_ownsDuplicate(true)
{
    setText(QObject::tr("Duplicate Pattern '%1'").arg(original->name()));
}

DuplicatePolylineCommand::~DuplicatePolylineCommand()
{
    if (m_ownsDuplicate && m_duplicate) {
        delete m_duplicate;
    }
}

void DuplicatePolylineCommand::undo()
{
    if (m_duplicate) {
        m_document->removeObjectDirect(m_duplicate);
        m_ownsDuplicate = true;
    }
}

void DuplicatePolylineCommand::redo()
{
    if (!m_duplicate) {
        // First execution: create the duplicate
        m_duplicate = m_original->clone();
        
        // Offset the duplicate slightly so it's visible
        m_duplicate->translate(QPointF(20, 20));
    }
    
    m_document->addObjectDirect(m_duplicate);
    m_ownsDuplicate = false;
}

} // namespace PatternCAD
