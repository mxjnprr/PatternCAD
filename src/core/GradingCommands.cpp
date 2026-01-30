/**
 * GradingCommands.cpp
 *
 * Implementation of grading commands
 */

#include "GradingCommands.h"
#include "geometry/Polyline.h"
#include "geometry/GradingSystem.h"

namespace PatternCAD {

SetGradingRulesCommand::SetGradingRulesCommand(Geometry::Polyline* polyline,
                                                GradingSystem* newGrading,
                                                QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_polyline(polyline)
    , m_hadOldGrading(false)
{
    setText(QObject::tr("Set Grading Rules"));
    
    // Save old grading state
    if (m_polyline->gradingSystem()) {
        m_oldGradingJson = m_polyline->gradingSystem()->toJson();
        m_hadOldGrading = true;
    }
    
    // Save new grading state
    if (newGrading) {
        m_newGradingJson = newGrading->toJson();
    }
}

SetGradingRulesCommand::~SetGradingRulesCommand()
{
}

void SetGradingRulesCommand::undo()
{
    if (m_hadOldGrading) {
        GradingSystem* restored = GradingSystem::fromJson(m_oldGradingJson);
        m_polyline->setGradingSystem(restored);
    } else {
        m_polyline->setGradingSystem(nullptr);
    }
}

void SetGradingRulesCommand::redo()
{
    if (!m_newGradingJson.isEmpty()) {
        GradingSystem* newGrading = GradingSystem::fromJson(m_newGradingJson);
        m_polyline->setGradingSystem(newGrading);
    } else {
        m_polyline->setGradingSystem(nullptr);
    }
}

} // namespace PatternCAD
