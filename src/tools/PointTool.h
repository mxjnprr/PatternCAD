/**
 * PointTool.h
 *
 * Tool for placing point objects
 */

#ifndef PATTERNCAD_POINTTOOL_H
#define PATTERNCAD_POINTTOOL_H

#include "Tool.h"

namespace PatternCAD {
namespace Tools {

/**
 * PointTool provides point placement functionality:
 * - Click to place a point
 * - Snap to grid support
 * - Immediate creation on click
 */
class PointTool : public Tool
{
    Q_OBJECT

public:
    explicit PointTool(QObject* parent = nullptr);
    ~PointTool() override;

    // Tool identification
    QString name() const override;
    QString description() const override;

    // Tool lifecycle
    void activate() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
};

} // namespace Tools
} // namespace PatternCAD

#endif // PATTERNCAD_POINTTOOL_H
