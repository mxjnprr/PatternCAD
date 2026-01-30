/**
 * GradingCommands.h
 *
 * Undo/redo commands for grading system
 */

#ifndef PATTERNCAD_GRADINGCOMMANDS_H
#define PATTERNCAD_GRADINGCOMMANDS_H

#include <QUndoCommand>
#include <QJsonObject>
#include <QVector>

namespace PatternCAD {

class GradingSystem;
class Document;

namespace Geometry {
    class Polyline;
}

/**
 * Command to set or modify grading rules on a polyline
 */
class SetGradingRulesCommand : public QUndoCommand
{
public:
    SetGradingRulesCommand(Geometry::Polyline* polyline, 
                           GradingSystem* newGrading,
                           QUndoCommand* parent = nullptr);
    ~SetGradingRulesCommand();

    void undo() override;
    void redo() override;

private:
    Geometry::Polyline* m_polyline;
    QJsonObject m_oldGradingJson;
    QJsonObject m_newGradingJson;
    bool m_hadOldGrading;
};

/**
 * Command to generate all graded sizes from a base pattern
 */
class GenerateGradedSizesCommand : public QUndoCommand
{
public:
    GenerateGradedSizesCommand(Document* document,
                                Geometry::Polyline* basePolyline,
                                QUndoCommand* parent = nullptr);
    ~GenerateGradedSizesCommand();

    void undo() override;
    void redo() override;
    
    // Get generated polylines
    QVector<Geometry::Polyline*> generatedPolylines() const { return m_generatedPolylines; }

private:
    Document* m_document;
    Geometry::Polyline* m_basePolyline;
    QVector<Geometry::Polyline*> m_generatedPolylines;
    bool m_firstRedo;
};

} // namespace PatternCAD

#endif // PATTERNCAD_GRADINGCOMMANDS_H
