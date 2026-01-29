/**
 * PreferencesDialog.cpp
 *
 * Implementation of PreferencesDialog
 */

#include "PreferencesDialog.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QMessageBox>

namespace PatternCAD {
namespace UI {

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
    , m_tabWidget(nullptr)
{
    setupUi();
    loadSettings();
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::setupUi()
{
    setWindowTitle(tr("Preferences"));
    setMinimumSize(600, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Tab widget
    m_tabWidget = new QTabWidget(this);
    createGeneralTab();
    createEditorTab();
    createFileIOTab();
    createAdvancedTab();
    mainLayout->addWidget(m_tabWidget);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_restoreDefaultsButton = new QPushButton(tr("Restore Defaults"), this);
    buttonLayout->addWidget(m_restoreDefaultsButton);

    buttonLayout->addStretch();

    m_okButton = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_applyButton = new QPushButton(tr("Apply"), this);

    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_applyButton);

    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(m_okButton, &QPushButton::clicked, this, &PreferencesDialog::onOk);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &PreferencesDialog::onApply);
    connect(m_restoreDefaultsButton, &QPushButton::clicked, this, &PreferencesDialog::onRestoreDefaults);
}

void PreferencesDialog::createGeneralTab()
{
    QWidget* generalTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(generalTab);

    // Units and Display group
    QGroupBox* unitsGroup = new QGroupBox(tr("Units and Display"));
    QFormLayout* unitsLayout = new QFormLayout(unitsGroup);

    m_unitCombo = new QComboBox();
    m_unitCombo->addItem(tr("Millimeters (mm)"));
    m_unitCombo->addItem(tr("Centimeters (cm)"));
    m_unitCombo->addItem(tr("Inches (in)"));
    unitsLayout->addRow(tr("Default Unit:"), m_unitCombo);

    m_themeCombo = new QComboBox();
    m_themeCombo->addItem(tr("Light"));
    m_themeCombo->addItem(tr("Dark"));
    m_themeCombo->addItem(tr("System"));
    unitsLayout->addRow(tr("Theme:"), m_themeCombo);

    layout->addWidget(unitsGroup);

    // Startup group
    QGroupBox* startupGroup = new QGroupBox(tr("Startup"));
    QVBoxLayout* startupLayout = new QVBoxLayout(startupGroup);

    m_showWelcomeCheck = new QCheckBox(tr("Show welcome screen on startup"));
    startupLayout->addWidget(m_showWelcomeCheck);

    m_checkUpdatesCheck = new QCheckBox(tr("Check for updates automatically"));
    startupLayout->addWidget(m_checkUpdatesCheck);

    layout->addWidget(startupGroup);
    layout->addStretch();

    m_tabWidget->addTab(generalTab, tr("General"));
}

void PreferencesDialog::createEditorTab()
{
    QWidget* editorTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(editorTab);

    // Grid Settings group
    QGroupBox* gridGroup = new QGroupBox(tr("Grid Settings"));
    QFormLayout* gridLayout = new QFormLayout(gridGroup);

    m_gridSpacingSpinBox = new QDoubleSpinBox();
    m_gridSpacingSpinBox->setRange(0.1, 1000.0);
    m_gridSpacingSpinBox->setSuffix(tr(" mm"));
    m_gridSpacingSpinBox->setDecimals(1);
    gridLayout->addRow(tr("Grid Spacing:"), m_gridSpacingSpinBox);

    m_gridAutoScaleCheck = new QCheckBox(tr("Auto-scale grid with zoom"));
    gridLayout->addRow("", m_gridAutoScaleCheck);

    m_gridColorButton = new QPushButton(tr("Choose Color..."));
    connect(m_gridColorButton, &QPushButton::clicked, this, &PreferencesDialog::onGridColorButton);
    gridLayout->addRow(tr("Grid Color:"), m_gridColorButton);

    layout->addWidget(gridGroup);

    // Snapping group
    QGroupBox* snapGroup = new QGroupBox(tr("Snapping"));
    QFormLayout* snapLayout = new QFormLayout(snapGroup);

    m_snapDistanceSpinBox = new QDoubleSpinBox();
    m_snapDistanceSpinBox->setRange(1.0, 50.0);
    m_snapDistanceSpinBox->setSuffix(tr(" pixels"));
    m_snapDistanceSpinBox->setDecimals(1);
    snapLayout->addRow(tr("Snap Distance:"), m_snapDistanceSpinBox);

    m_snapToGridCheck = new QCheckBox(tr("Snap to grid"));
    snapLayout->addRow("", m_snapToGridCheck);

    m_snapToObjectsCheck = new QCheckBox(tr("Snap to objects"));
    snapLayout->addRow("", m_snapToObjectsCheck);

    layout->addWidget(snapGroup);

    // Selection group
    QGroupBox* selectionGroup = new QGroupBox(tr("Selection"));
    QFormLayout* selectionLayout = new QFormLayout(selectionGroup);

    m_selectionColorButton = new QPushButton(tr("Choose Color..."));
    connect(m_selectionColorButton, &QPushButton::clicked, this, &PreferencesDialog::onSelectionColorButton);
    selectionLayout->addRow(tr("Selection Color:"), m_selectionColorButton);

    m_selectionLineWidthSpinBox = new QDoubleSpinBox();
    m_selectionLineWidthSpinBox->setRange(1.0, 10.0);
    m_selectionLineWidthSpinBox->setSuffix(tr(" pixels"));
    m_selectionLineWidthSpinBox->setDecimals(1);
    selectionLayout->addRow(tr("Line Width:"), m_selectionLineWidthSpinBox);

    layout->addWidget(selectionGroup);

    // Canvas group
    QGroupBox* canvasGroup = new QGroupBox(tr("Canvas"));
    QFormLayout* canvasLayout = new QFormLayout(canvasGroup);

    m_canvasColorButton = new QPushButton(tr("Choose Color..."));
    connect(m_canvasColorButton, &QPushButton::clicked, this, &PreferencesDialog::onCanvasColorButton);
    canvasLayout->addRow(tr("Background Color:"), m_canvasColorButton);

    m_antiAliasingCheck = new QCheckBox(tr("Enable anti-aliasing"));
    canvasLayout->addRow("", m_antiAliasingCheck);

    layout->addWidget(canvasGroup);
    layout->addStretch();

    m_tabWidget->addTab(editorTab, tr("Editor"));
}

void PreferencesDialog::createFileIOTab()
{
    QWidget* fileIOTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(fileIOTab);

    // Auto-save group
    QGroupBox* autoSaveGroup = new QGroupBox(tr("Auto-Save"));
    QFormLayout* autoSaveLayout = new QFormLayout(autoSaveGroup);

    m_autoSaveCheck = new QCheckBox(tr("Enable auto-save"));
    autoSaveLayout->addRow("", m_autoSaveCheck);

    m_autoSaveIntervalSpinBox = new QSpinBox();
    m_autoSaveIntervalSpinBox->setRange(1, 60);
    m_autoSaveIntervalSpinBox->setSuffix(tr(" minutes"));
    autoSaveLayout->addRow(tr("Interval:"), m_autoSaveIntervalSpinBox);

    m_autoSaveVersionsSpinBox = new QSpinBox();
    m_autoSaveVersionsSpinBox->setRange(1, 50);
    m_autoSaveVersionsSpinBox->setSuffix(tr(" files"));
    autoSaveLayout->addRow(tr("Keep Versions:"), m_autoSaveVersionsSpinBox);

    layout->addWidget(autoSaveGroup);

    // Recent Files group
    QGroupBox* recentGroup = new QGroupBox(tr("Recent Files"));
    QFormLayout* recentLayout = new QFormLayout(recentGroup);

    m_recentFilesCountSpinBox = new QSpinBox();
    m_recentFilesCountSpinBox->setRange(1, 30);
    m_recentFilesCountSpinBox->setSuffix(tr(" files"));
    recentLayout->addRow(tr("Show Recent:"), m_recentFilesCountSpinBox);

    layout->addWidget(recentGroup);

    // File Format group
    QGroupBox* formatGroup = new QGroupBox(tr("File Format"));
    QVBoxLayout* formatLayout = new QVBoxLayout(formatGroup);

    m_compressNativeCheck = new QCheckBox(tr("Compress native files (.patterncad)"));
    formatLayout->addWidget(m_compressNativeCheck);

    layout->addWidget(formatGroup);
    layout->addStretch();

    m_tabWidget->addTab(fileIOTab, tr("File I/O"));
}

void PreferencesDialog::createAdvancedTab()
{
    QWidget* advancedTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(advancedTab);

    // Performance group
    QGroupBox* perfGroup = new QGroupBox(tr("Performance"));
    QFormLayout* perfLayout = new QFormLayout(perfGroup);

    m_undoHistoryLimitSpinBox = new QSpinBox();
    m_undoHistoryLimitSpinBox->setRange(10, 1000);
    m_undoHistoryLimitSpinBox->setSuffix(tr(" MB"));
    perfLayout->addRow(tr("Undo History Limit:"), m_undoHistoryLimitSpinBox);

    m_useOpenGLCheck = new QCheckBox(tr("Use OpenGL rendering (restart required)"));
    perfLayout->addRow("", m_useOpenGLCheck);

    m_maxRenderThreadsSpinBox = new QSpinBox();
    m_maxRenderThreadsSpinBox->setRange(1, 16);
    perfLayout->addRow(tr("Max Render Threads:"), m_maxRenderThreadsSpinBox);

    layout->addWidget(perfGroup);

    // Debugging group
    QGroupBox* debugGroup = new QGroupBox(tr("Debugging"));
    QVBoxLayout* debugLayout = new QVBoxLayout(debugGroup);

    m_showFPSCheck = new QCheckBox(tr("Show FPS counter"));
    debugLayout->addWidget(m_showFPSCheck);

    m_logDebugCheck = new QCheckBox(tr("Log debug messages to console"));
    debugLayout->addWidget(m_logDebugCheck);

    layout->addWidget(debugGroup);
    layout->addStretch();

    m_tabWidget->addTab(advancedTab, tr("Advanced"));
}

void PreferencesDialog::loadSettings()
{
    SettingsManager& settings = SettingsManager::instance();

    // General
    GeneralSettings general = settings.general();
    m_unitCombo->setCurrentIndex(static_cast<int>(general.defaultUnit));
    m_themeCombo->setCurrentIndex(static_cast<int>(general.theme));
    m_showWelcomeCheck->setChecked(general.showWelcomeScreen);
    m_checkUpdatesCheck->setChecked(general.checkForUpdates);

    // Editor
    EditorSettings editor = settings.editor();
    m_gridSpacingSpinBox->setValue(editor.gridSpacing);
    m_gridAutoScaleCheck->setChecked(editor.gridAutoScale);
    m_gridColor = editor.gridColor;
    updateColorButton(m_gridColorButton, m_gridColor);

    m_snapDistanceSpinBox->setValue(editor.snapDistance);
    m_snapToGridCheck->setChecked(editor.snapToGrid);
    m_snapToObjectsCheck->setChecked(editor.snapToObjects);

    m_selectionColor = editor.selectionColor;
    updateColorButton(m_selectionColorButton, m_selectionColor);
    m_selectionLineWidthSpinBox->setValue(editor.selectionLineWidth);

    m_canvasColor = editor.canvasBackgroundColor;
    updateColorButton(m_canvasColorButton, m_canvasColor);
    m_antiAliasingCheck->setChecked(editor.antiAliasing);

    // File I/O
    FileIOSettings fileIO = settings.fileIO();
    m_autoSaveCheck->setChecked(fileIO.autoSaveEnabled);
    m_autoSaveIntervalSpinBox->setValue(fileIO.autoSaveInterval);
    m_autoSaveVersionsSpinBox->setValue(fileIO.autoSaveVersions);
    m_recentFilesCountSpinBox->setValue(fileIO.recentFilesCount);
    m_compressNativeCheck->setChecked(fileIO.compressNativeFormat);

    // Advanced
    AdvancedSettings advanced = settings.advanced();
    m_undoHistoryLimitSpinBox->setValue(advanced.undoHistoryMemoryLimit);
    m_useOpenGLCheck->setChecked(advanced.useOpenGLRendering);
    m_maxRenderThreadsSpinBox->setValue(advanced.maxRenderThreads);
    m_showFPSCheck->setChecked(advanced.showFPS);
    m_logDebugCheck->setChecked(advanced.logDebugMessages);
}

void PreferencesDialog::saveSettings()
{
    SettingsManager& settings = SettingsManager::instance();

    // General
    GeneralSettings general;
    general.defaultUnit = static_cast<GeneralSettings::Unit>(m_unitCombo->currentIndex());
    general.theme = static_cast<GeneralSettings::Theme>(m_themeCombo->currentIndex());
    general.showWelcomeScreen = m_showWelcomeCheck->isChecked();
    general.checkForUpdates = m_checkUpdatesCheck->isChecked();
    settings.setGeneral(general);

    // Editor
    EditorSettings editor;
    editor.gridSpacing = m_gridSpacingSpinBox->value();
    editor.gridAutoScale = m_gridAutoScaleCheck->isChecked();
    editor.gridColor = m_gridColor;
    editor.snapDistance = m_snapDistanceSpinBox->value();
    editor.snapToGrid = m_snapToGridCheck->isChecked();
    editor.snapToObjects = m_snapToObjectsCheck->isChecked();
    editor.selectionColor = m_selectionColor;
    editor.selectionLineWidth = m_selectionLineWidthSpinBox->value();
    editor.canvasBackgroundColor = m_canvasColor;
    editor.antiAliasing = m_antiAliasingCheck->isChecked();
    settings.setEditor(editor);

    // File I/O
    FileIOSettings fileIO;
    fileIO.autoSaveEnabled = m_autoSaveCheck->isChecked();
    fileIO.autoSaveInterval = m_autoSaveIntervalSpinBox->value();
    fileIO.autoSaveVersions = m_autoSaveVersionsSpinBox->value();
    fileIO.recentFilesCount = m_recentFilesCountSpinBox->value();
    fileIO.compressNativeFormat = m_compressNativeCheck->isChecked();
    settings.setFileIO(fileIO);

    // Advanced
    AdvancedSettings advanced;
    advanced.undoHistoryMemoryLimit = m_undoHistoryLimitSpinBox->value();
    advanced.useOpenGLRendering = m_useOpenGLCheck->isChecked();
    advanced.maxRenderThreads = m_maxRenderThreadsSpinBox->value();
    advanced.showFPS = m_showFPSCheck->isChecked();
    advanced.logDebugMessages = m_logDebugCheck->isChecked();
    settings.setAdvanced(advanced);

    settings.save();
}

void PreferencesDialog::applySettings()
{
    saveSettings();
    // Settings will be applied via signals from SettingsManager
}

void PreferencesDialog::onOk()
{
    saveSettings();
    accept();
}

void PreferencesDialog::onApply()
{
    applySettings();
}

void PreferencesDialog::onRestoreDefaults()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Restore Defaults"),
        tr("Are you sure you want to restore all settings to their default values?"),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        SettingsManager::instance().restoreDefaults();
        loadSettings();
    }
}

void PreferencesDialog::onGridColorButton()
{
    QColor color = QColorDialog::getColor(m_gridColor, this, tr("Choose Grid Color"));
    if (color.isValid()) {
        m_gridColor = color;
        updateColorButton(m_gridColorButton, m_gridColor);
    }
}

void PreferencesDialog::onSelectionColorButton()
{
    QColor color = QColorDialog::getColor(m_selectionColor, this, tr("Choose Selection Color"));
    if (color.isValid()) {
        m_selectionColor = color;
        updateColorButton(m_selectionColorButton, m_selectionColor);
    }
}

void PreferencesDialog::onCanvasColorButton()
{
    QColor color = QColorDialog::getColor(m_canvasColor, this, tr("Choose Canvas Background Color"));
    if (color.isValid()) {
        m_canvasColor = color;
        updateColorButton(m_canvasColorButton, m_canvasColor);
    }
}

void PreferencesDialog::updateColorButton(QPushButton* button, const QColor& color)
{
    if (button && color.isValid()) {
        QPixmap pixmap(32, 16);
        pixmap.fill(color);
        button->setIcon(QIcon(pixmap));
        button->setText(color.name());
    }
}

} // namespace UI
} // namespace PatternCAD
