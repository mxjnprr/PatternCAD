/**
 * GradingDialog.h
 *
 * Dialog for configuring grading rules and sizes
 */

#ifndef PATTERNCAD_GRADINGDIALOG_H
#define PATTERNCAD_GRADINGDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

namespace PatternCAD {

class GradingSystem;

namespace Geometry {
    class Polyline;
}

/**
 * Dialog for configuring grading sizes and rules.
 */
class GradingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GradingDialog(Geometry::Polyline* polyline, QWidget* parent = nullptr);
    ~GradingDialog();

    // Get the configured grading system
    GradingSystem* gradingSystem() const { return m_gradingSystem; }

private slots:
    void onAddSize();
    void onRemoveSize();
    void onSetStandardSizes();
    void onSetNumericSizes();
    void onBaseSizeChanged(int index);
    
    void onAddRule();
    void onRemoveRule();
    void onRuleChanged(int row, int column);
    
    void onAccept();

private:
    void setupUI();
    void populateSizesTable();
    void populateRulesTable();
    void updateVertexCombo();
    
    Geometry::Polyline* m_polyline;
    GradingSystem* m_gradingSystem;
    
    // Sizes UI
    QTableWidget* m_sizesTable;
    QComboBox* m_baseSizeCombo;
    QPushButton* m_addSizeBtn;
    QPushButton* m_removeSizeBtn;
    QPushButton* m_standardSizesBtn;
    QPushButton* m_numericSizesBtn;
    
    // Rules UI
    QTableWidget* m_rulesTable;
    QComboBox* m_vertexCombo;
    QPushButton* m_addRuleBtn;
    QPushButton* m_removeRuleBtn;
    
    // Info
    QLabel* m_infoLabel;
};

} // namespace PatternCAD

#endif // PATTERNCAD_GRADINGDIALOG_H
