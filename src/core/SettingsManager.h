/**
 * SettingsManager.h
 *
 * Singleton for managing application settings/preferences
 */

#ifndef PATTERNCAD_SETTINGSMANAGER_H
#define PATTERNCAD_SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QString>

namespace PatternCAD {

/**
 * General application settings
 */
struct GeneralSettings {
    enum Unit { Millimeters = 0, Centimeters = 1, Inches = 2 };
    enum Theme { Light = 0, Dark = 1, System = 2 };

    Unit defaultUnit = Millimeters;
    Theme theme = System;
    bool showWelcomeScreen = false;
    bool checkForUpdates = true;
};

/**
 * Editor/canvas settings
 */
struct EditorSettings {
    // Grid
    double gridSpacing = 10.0;  // mm
    bool gridAutoScale = true;
    QColor gridColor = QColor(220, 220, 220);

    // Snapping
    double snapDistance = 5.0;  // pixels
    bool snapToGrid = false;
    bool snapToObjects = true;

    // Selection
    QColor selectionColor = QColor(0, 120, 215);
    double selectionLineWidth = 2.0;

    // Canvas
    QColor canvasBackgroundColor = Qt::white;
    bool antiAliasing = true;
};

/**
 * File I/O settings
 */
struct FileIOSettings {
    // Auto-save
    bool autoSaveEnabled = true;
    int autoSaveInterval = 5;  // minutes
    int autoSaveVersions = 10;  // keep last N versions

    // Recent files
    int recentFilesCount = 10;

    // Default directories
    QString lastOpenDirectory;
    QString lastSaveDirectory;
    QString lastExportDirectory;

    // File format
    bool compressNativeFormat = true;
};

/**
 * Advanced/performance settings
 */
struct AdvancedSettings {
    // Performance
    int undoHistoryMemoryLimit = 200;  // MB
    bool useOpenGLRendering = true;
    int maxRenderThreads = 4;

    // Debugging
    bool showFPS = false;
    bool logDebugMessages = false;
};

/**
 * SettingsManager provides centralized access to application settings.
 * Settings are automatically persisted using QSettings.
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager& instance();

    // Load/Save
    void load();
    void save();
    void restoreDefaults();

    // Getters
    GeneralSettings general() const { return m_general; }
    EditorSettings editor() const { return m_editor; }
    FileIOSettings fileIO() const { return m_fileIO; }
    AdvancedSettings advanced() const { return m_advanced; }

    // Setters
    void setGeneral(const GeneralSettings& settings);
    void setEditor(const EditorSettings& settings);
    void setFileIO(const FileIOSettings& settings);
    void setAdvanced(const AdvancedSettings& settings);

signals:
    void settingsChanged();
    void generalSettingsChanged();
    void editorSettingsChanged();
    void fileIOSettingsChanged();
    void advancedSettingsChanged();

private:
    SettingsManager();
    ~SettingsManager() = default;

    // Non-copyable
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    void loadGeneral();
    void loadEditor();
    void loadFileIO();
    void loadAdvanced();

    void saveGeneral();
    void saveEditor();
    void saveFileIO();
    void saveAdvanced();

    QSettings m_settings;
    GeneralSettings m_general;
    EditorSettings m_editor;
    FileIOSettings m_fileIO;
    AdvancedSettings m_advanced;
};

} // namespace PatternCAD

#endif // PATTERNCAD_SETTINGSMANAGER_H
