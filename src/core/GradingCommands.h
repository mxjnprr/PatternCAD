/**
 * GradingCommands.h
 *
 * Undo/redo commands for grading system
 */

#ifndef PATTERNCAD_GRADINGCOMMANDS_H
#define PATTERNCAD_GRADINGCOMMANDS_H

#include <QUndoCommand>
#include <QJsonObject>

namespace PatternCAD {

class GradingSystem;

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

} // namespace PatternCAD

#endif // PATTERNCAD_GRADINGCOMMANDS_H
