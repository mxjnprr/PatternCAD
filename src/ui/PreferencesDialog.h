/**
 * PreferencesDialog.h
 *
 * Dialog for application preferences/settings
 */

#ifndef PATTERNCAD_PREFERENCESDIALOG_H
#define PATTERNCAD_PREFERENCESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

namespace PatternCAD {
namespace UI {

/**
 * PreferencesDialog provides a user interface for configuring
 * application settings across multiple categories.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);
    ~PreferencesDialog();

private slots:
    void onApply();
    void onOk();
    void onRestoreDefaults();
    void onGridColorButton();
    void onSelectionColorButton();
    void onCanvasColorButton();
    void onAutoSaveLocationChanged(int index);
    void onBrowseAutoSaveDirectory();

private:
    void setupUi();
    void createGeneralTab();
    void createEditorTab();
    void createFileIOTab();
    void createAdvancedTab();

    void loadSettings();
    void saveSettings();
    void applySettings();
    void updateColorButton(QPushButton* button, const QColor& color);

    // Main widget
    QTabWidget* m_tabWidget;

    // General tab widgets
    QComboBox* m_unitCombo;
    QComboBox* m_themeCombo;
    QCheckBox* m_showWelcomeCheck;
    QCheckBox* m_checkUpdatesCheck;

    // Editor tab widgets
    QDoubleSpinBox* m_gridSpacingSpinBox;
    QCheckBox* m_gridAutoScaleCheck;
    QPushButton* m_gridColorButton;
    QColor m_gridColor;

    QDoubleSpinBox* m_snapDistanceSpinBox;
    QCheckBox* m_snapToGridCheck;
    QCheckBox* m_snapToObjectsCheck;

    QPushButton* m_selectionColorButton;
    QColor m_selectionColor;
    QDoubleSpinBox* m_selectionLineWidthSpinBox;

    QPushButton* m_canvasColorButton;
    QColor m_canvasColor;
    QCheckBox* m_antiAliasingCheck;

    // File I/O tab widgets
    QCheckBox* m_autoSaveCheck;
    QSpinBox* m_autoSaveIntervalSpinBox;
    QSpinBox* m_autoSaveVersionsSpinBox;
    QComboBox* m_autoSaveLocationCombo;
    QLineEdit* m_autoSaveCustomDirEdit;
    QPushButton* m_autoSaveCustomDirButton;
    QSpinBox* m_recentFilesCountSpinBox;
    QCheckBox* m_compressNativeCheck;

    // Advanced tab widgets
    QSpinBox* m_undoHistoryLimitSpinBox;
    QCheckBox* m_useOpenGLCheck;
    QSpinBox* m_maxRenderThreadsSpinBox;
    QCheckBox* m_showFPSCheck;
    QCheckBox* m_logDebugCheck;

    // Dialog buttons
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
    QPushButton* m_restoreDefaultsButton;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_PREFERENCESDIALOG_H
