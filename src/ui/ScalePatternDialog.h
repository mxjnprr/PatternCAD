/**
 * ScalePatternDialog.h
 *
 * Dialog for scaling patterns by percentage or to specific dimensions
 */

#ifndef PATTERNCAD_SCALEPATTERNDIALOG_H
#define PATTERNCAD_SCALEPATTERNDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QButtonGroup>

namespace PatternCAD {

namespace Geometry {
    class Polyline;
}

/**
 * Dialog for scaling patterns with precise control.
 * Supports:
 * - Scale by percentage (e.g., 110%)
 * - Scale to fit specific width or height
 * - Uniform vs non-uniform scaling
 * - Option to scale seam allowance and notch depths
 */
class ScalePatternDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScalePatternDialog(Geometry::Polyline* polyline, QWidget* parent = nullptr);
    ~ScalePatternDialog();

    // Results
    double scaleX() const { return m_scaleX; }
    double scaleY() const { return m_scaleY; }
    bool scaleSeamAllowance() const;
    bool scaleNotchDepths() const;

private slots:
    void onModeChanged();
    void onPercentageChanged();
    void onDimensionChanged();
    void onUniformChanged(bool uniform);
    void updatePreview();
    void onAccept();

private:
    void setupUI();
    void calculateScaleFromDimension();

    Geometry::Polyline* m_polyline;
    
    // Current pattern dimensions
    double m_originalWidth;
    double m_originalHeight;
    
    // Scale results
    double m_scaleX;
    double m_scaleY;
    
    // UI - Mode selection
    QRadioButton* m_percentageMode;
    QRadioButton* m_dimensionMode;
    QButtonGroup* m_modeGroup;
    
    // UI - Percentage scaling
    QDoubleSpinBox* m_percentageXSpin;
    QDoubleSpinBox* m_percentageYSpin;
    QCheckBox* m_uniformCheck;
    
    // UI - Dimension scaling
    QDoubleSpinBox* m_targetWidthSpin;
    QDoubleSpinBox* m_targetHeightSpin;
    QRadioButton* m_fitWidth;
    QRadioButton* m_fitHeight;
    
    // UI - Options
    QCheckBox* m_scaleSeamCheck;
    QCheckBox* m_scaleNotchCheck;
    
    // UI - Preview
    QLabel* m_previewLabel;
    QLabel* m_resultLabel;
};

} // namespace PatternCAD

#endif // PATTERNCAD_SCALEPATTERNDIALOG_H
