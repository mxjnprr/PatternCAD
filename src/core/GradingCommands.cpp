/**
 * GradingCommands.cpp
 *
 * Implementation of grading commands
 */

#include "GradingCommands.h"
#include "Document.h"
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

// --- GenerateGradedSizesCommand ---

GenerateGradedSizesCommand::GenerateGradedSizesCommand(Document* document,
                                                        Geometry::Polyline* basePolyline,
                                                        QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_basePolyline(basePolyline)
    , m_firstRedo(true)
{
    GradingSystem* grading = basePolyline->gradingSystem();
    int sizeCount = grading ? grading->sizeCount() : 0;
    setText(QObject::tr("Generate %1 Graded Sizes").arg(sizeCount));
}

GenerateGradedSizesCommand::~GenerateGradedSizesCommand()
{
    // Don't delete polylines - they are owned by the document
}

void GenerateGradedSizesCommand::undo()
{
    // Remove all generated polylines from document
    for (Geometry::Polyline* polyline : m_generatedPolylines) {
        m_document->removeObject(polyline);
    }
}

void GenerateGradedSizesCommand::redo()
{
    GradingSystem* grading = m_basePolyline->gradingSystem();
    if (!grading || grading->sizeCount() == 0) {
        return;
    }
    
    if (m_firstRedo) {
        // First time: generate all sizes
        m_firstRedo = false;
        
        int baseSizeIndex = grading->baseSizeIndex();
        
        for (int i = 0; i < grading->sizeCount(); ++i) {
            // Skip the base size - it's already the original
            if (i == baseSizeIndex) {
                continue;
            }
            
            // Generate graded version
            Geometry::Polyline* graded = grading->applyToSize(m_basePolyline, i);
            if (graded) {
                m_generatedPolylines.append(graded);
                m_document->addObject(graded);
            }
        }
    } else {
        // Re-add previously generated polylines
        for (Geometry::Polyline* polyline : m_generatedPolylines) {
            m_document->addObject(polyline);
        }
    }
}

} // namespace PatternCAD
