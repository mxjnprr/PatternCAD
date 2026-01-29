/**
 * Units.cpp
 *
 * Implementation of Units
 */

#include "Units.h"
#include "Project.h"

namespace PatternCAD {

// Default to centimeters (common in pattern-making)
Unit Units::s_currentUnit = Unit::Centimeters;

Unit Units::currentUnit()
{
    return s_currentUnit;
}

void Units::setCurrentUnit(Unit unit)
{
    s_currentUnit = unit;
}

double Units::toInternal(double value, Unit unit)
{
    switch (unit) {
        case Unit::Millimeters:
            return value;
        case Unit::Centimeters:
            return value * 10.0;
        case Unit::Inches:
            return value * 25.4;
    }
    return value;
}

double Units::fromInternal(double value, Unit unit)
{
    switch (unit) {
        case Unit::Millimeters:
            return value;
        case Unit::Centimeters:
            return value / 10.0;
        case Unit::Inches:
            return value / 25.4;
    }
    return value;
}

double Units::toCurrentUnit(double valueInMm)
{
    return fromInternal(valueInMm, s_currentUnit);
}

double Units::fromCurrentUnit(double value)
{
    return toInternal(value, s_currentUnit);
}

QString Units::formatLength(double lengthInMm, int decimals)
{
    double value = toCurrentUnit(lengthInMm);
    return QString("%1 %2").arg(value, 0, 'f', decimals).arg(currentUnitSymbol());
}

QString Units::unitSymbol(Unit unit)
{
    switch (unit) {
        case Unit::Millimeters:
            return "mm";
        case Unit::Centimeters:
            return "cm";
        case Unit::Inches:
            return "in";
    }
    return "";
}

QString Units::currentUnitSymbol()
{
    return unitSymbol(s_currentUnit);
}

} // namespace PatternCAD
