/**
 * Units.h
 *
 * Unit system for measurements
 */

#ifndef PATTERNCAD_UNITS_H
#define PATTERNCAD_UNITS_H

#include <QString>

namespace PatternCAD {

// Unit enum is defined in Project.h
enum class Unit;

/**
 * Units class provides unit conversion and formatting
 */
class Units
{
public:
    // Current unit
    static Unit currentUnit();
    static void setCurrentUnit(Unit unit);

    // Conversion (internal storage is always in millimeters)
    static double toInternal(double value, Unit unit);
    static double fromInternal(double value, Unit unit);
    static double toCurrentUnit(double valueInMm);
    static double fromCurrentUnit(double value);

    // Formatting
    static QString formatLength(double lengthInMm, int decimals = 1);
    static QString unitSymbol(Unit unit);
    static QString currentUnitSymbol();

private:
    static Unit s_currentUnit;
};

} // namespace PatternCAD

#endif // PATTERNCAD_UNITS_H
