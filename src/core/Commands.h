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

/**
 * Alignment modes for AlignObjectsCommand
 */
enum class AlignMode {
    Left,
    Right,
    Top,
    Bottom,
    CenterHorizontal,
    CenterVertical
};

/**
 * Distribution modes for DistributeObjectsCommand
 */
enum class DistributeMode {
    Horizontal,
    Vertical
};

namespace Geometry {
    class GeometryObject;
    class Polyline;
}

// Forward declarations for pattern features
class Notch;
class MatchPoint;

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

/**
 * RotateObjectsCommand - Command to rotate multiple objects
 */
class RotateObjectsCommand : public QUndoCommand
{
public:
    RotateObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                         double angleDegrees,
                         const QPointF& center,
                         QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QList<Geometry::GeometryObject*> m_objects;
    double m_angleDegrees;
    QPointF m_center;
};

/**
 * MirrorObjectsCommand - Command to create mirrored copies of objects
 */
class MirrorObjectsCommand : public QUndoCommand
{
public:
    MirrorObjectsCommand(Document* document,
                         const QList<Geometry::GeometryObject*>& objects,
                         const QPointF& axisPoint1,
                         const QPointF& axisPoint2,
                         QUndoCommand* parent = nullptr);
    ~MirrorObjectsCommand();

    void undo() override;
    void redo() override;

    QList<Geometry::GeometryObject*> mirroredObjects() const { return m_mirroredObjects; }

private:
    Document* m_document;
    QList<Geometry::GeometryObject*> m_originalObjects;
    QList<Geometry::GeometryObject*> m_mirroredObjects;
    QPointF m_axisPoint1;
    QPointF m_axisPoint2;
    bool m_firstRedo;
};

/**
 * ScaleObjectsCommand - Command to scale multiple objects
 */
class ScaleObjectsCommand : public QUndoCommand
{
public:
    ScaleObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                        double scaleX,
                        double scaleY,
                        const QPointF& origin,
                        QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QList<Geometry::GeometryObject*> m_objects;
    double m_scaleX;
    double m_scaleY;
    QPointF m_origin;
};

/**
 * AlignObjectsCommand - Command to align multiple objects
 */
class AlignObjectsCommand : public QUndoCommand
{
public:
    AlignObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                        AlignMode mode,
                        QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    struct ObjectOffset {
        Geometry::GeometryObject* object;
        QPointF originalOffset;
    };

    QList<ObjectOffset> m_objects;
    AlignMode m_mode;
};

/**
 * DistributeObjectsCommand - Command to distribute multiple objects evenly
 */
class DistributeObjectsCommand : public QUndoCommand
{
public:
    DistributeObjectsCommand(const QList<Geometry::GeometryObject*>& objects,
                            DistributeMode mode,
                            QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    struct ObjectOffset {
        Geometry::GeometryObject* object;
        QPointF originalOffset;
    };

    QList<ObjectOffset> m_objects;
    DistributeMode m_mode;
};

/**
 * DeleteVertexCommand - Command to delete a vertex from a polyline
 */
class DeleteVertexCommand : public QUndoCommand
{
public:
    DeleteVertexCommand(Geometry::GeometryObject* object, int vertexIndex,
                       QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Geometry::GeometryObject* m_object;
    int m_vertexIndex;
    QPointF m_position;
    int m_vertexType;  // Stored as int to avoid including Polyline.h
    double m_incomingTension;
    double m_outgoingTension;
    QPointF m_tangent;
};

/**
 * ChangeVertexTypeCommand - Command to change a vertex type (Sharp/Smooth)
 */
class ChangeVertexTypeCommand : public QUndoCommand
{
public:
    ChangeVertexTypeCommand(Geometry::GeometryObject* object, int vertexIndex,
                           int oldType, int newType,
                           QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Geometry::GeometryObject* m_object;
    int m_vertexIndex;
    int m_oldType;  // Stored as int to avoid including Polyline.h
    int m_newType;
};

// =============================================================================
// Story 004-02: Notch Commands
// =============================================================================

/**
 * AddNotchCommand - Command to add a notch to a polyline
 */
class AddNotchCommand : public QUndoCommand
{
public:
    AddNotchCommand(Geometry::Polyline* polyline, Notch* notch,
                    QUndoCommand* parent = nullptr);
    ~AddNotchCommand();

    void undo() override;
    void redo() override;

private:
    Geometry::Polyline* m_polyline;
    Notch* m_notch;
    bool m_ownsNotch;
};

/**
 * RemoveNotchCommand - Command to remove a notch from a polyline
 */
class RemoveNotchCommand : public QUndoCommand
{
public:
    RemoveNotchCommand(Geometry::Polyline* polyline, Notch* notch,
                       QUndoCommand* parent = nullptr);
    ~RemoveNotchCommand();

    void undo() override;
    void redo() override;

private:
    Geometry::Polyline* m_polyline;
    Notch* m_notch;
    bool m_ownsNotch;
};

/**
 * ModifyNotchCommand - Command to modify notch properties
 */
class ModifyNotchCommand : public QUndoCommand
{
public:
    ModifyNotchCommand(Notch* notch, int newStyle, double newDepth,
                       int newSegmentIndex, double newPosition,
                       QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Notch* m_notch;
    int m_oldStyle, m_newStyle;
    double m_oldDepth, m_newDepth;
    int m_oldSegmentIndex, m_newSegmentIndex;
    double m_oldPosition, m_newPosition;
};

// =============================================================================
// Story 004-03: MatchPoint Commands
// =============================================================================

/**
 * AddMatchPointCommand - Command to add a match point to a polyline
 */
class AddMatchPointCommand : public QUndoCommand
{
public:
    AddMatchPointCommand(Geometry::Polyline* polyline, MatchPoint* matchPoint,
                         QUndoCommand* parent = nullptr);
    ~AddMatchPointCommand();

    void undo() override;
    void redo() override;

private:
    Geometry::Polyline* m_polyline;
    MatchPoint* m_matchPoint;
    bool m_ownsMatchPoint;
};

/**
 * RemoveMatchPointCommand - Command to remove a match point from a polyline
 */
class RemoveMatchPointCommand : public QUndoCommand
{
public:
    RemoveMatchPointCommand(Geometry::Polyline* polyline, MatchPoint* matchPoint,
                            QUndoCommand* parent = nullptr);
    ~RemoveMatchPointCommand();

    void undo() override;
    void redo() override;

private:
    Geometry::Polyline* m_polyline;
    MatchPoint* m_matchPoint;
    bool m_ownsMatchPoint;
    QVector<MatchPoint*> m_linkedPointsBackup;  // For restoring links on undo
};

/**
 * ModifyMatchPointCommand - Command to modify match point properties
 */
class ModifyMatchPointCommand : public QUndoCommand
{
public:
    ModifyMatchPointCommand(MatchPoint* matchPoint, const QString& newLabel,
                            int newSegmentIndex, double newSegmentPosition,
                            QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    MatchPoint* m_matchPoint;
    QString m_oldLabel, m_newLabel;
    int m_oldSegmentIndex, m_newSegmentIndex;
    double m_oldSegmentPosition, m_newSegmentPosition;
};

/**
 * LinkMatchPointsCommand - Command to link/unlink two match points
 */
class LinkMatchPointsCommand : public QUndoCommand
{
public:
    LinkMatchPointsCommand(MatchPoint* pointA, MatchPoint* pointB, bool link,
                           QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    MatchPoint* m_pointA;
    MatchPoint* m_pointB;
    bool m_link;  // true = link, false = unlink
};

// =============================================================================
// Story 004-07: Duplicate Pattern Command
// =============================================================================

/**
 * DuplicatePolylineCommand - Command to duplicate a polyline (pattern piece)
 */
class DuplicatePolylineCommand : public QUndoCommand
{
public:
    DuplicatePolylineCommand(Document* document, Geometry::Polyline* original,
                             QUndoCommand* parent = nullptr);
    ~DuplicatePolylineCommand();

    void undo() override;
    void redo() override;

    Geometry::Polyline* duplicatedPolyline() const { return m_duplicate; }

private:
    Document* m_document;
    Geometry::Polyline* m_original;
    Geometry::Polyline* m_duplicate;
    bool m_ownsDuplicate;
};

} // namespace PatternCAD

#endif // PATTERNCAD_COMMANDS_H
