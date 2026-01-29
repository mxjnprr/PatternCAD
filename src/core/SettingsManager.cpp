/**
 * SettingsManager.cpp
 *
 * Implementation of SettingsManager
 */

#include "SettingsManager.h"
#include <QCoreApplication>

namespace PatternCAD {

SettingsManager::SettingsManager()
    : m_settings(QCoreApplication::organizationName(), QCoreApplication::applicationName())
{
    // Load settings on construction
    load();
}

SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

void SettingsManager::load()
{
    loadGeneral();
    loadEditor();
    loadFileIO();
    loadAdvanced();
}

void SettingsManager::save()
{
    saveGeneral();
    saveEditor();
    saveFileIO();
    saveAdvanced();

    m_settings.sync();
}

void SettingsManager::restoreDefaults()
{
    m_general = GeneralSettings();
    m_editor = EditorSettings();
    m_fileIO = FileIOSettings();
    m_advanced = AdvancedSettings();

    save();
    emit settingsChanged();
    emit generalSettingsChanged();
    emit editorSettingsChanged();
    emit fileIOSettingsChanged();
    emit advancedSettingsChanged();
}

void SettingsManager::setGeneral(const GeneralSettings& settings)
{
    if (m_general.defaultUnit != settings.defaultUnit ||
        m_general.theme != settings.theme ||
        m_general.showWelcomeScreen != settings.showWelcomeScreen ||
        m_general.checkForUpdates != settings.checkForUpdates) {

        m_general = settings;
        saveGeneral();
        emit generalSettingsChanged();
        emit settingsChanged();
    }
}

void SettingsManager::setEditor(const EditorSettings& settings)
{
    m_editor = settings;
    saveEditor();
    emit editorSettingsChanged();
    emit settingsChanged();
}

void SettingsManager::setFileIO(const FileIOSettings& settings)
{
    m_fileIO = settings;
    saveFileIO();
    emit fileIOSettingsChanged();
    emit settingsChanged();
}

void SettingsManager::setAdvanced(const AdvancedSettings& settings)
{
    m_advanced = settings;
    saveAdvanced();
    emit advancedSettingsChanged();
    emit settingsChanged();
}

// Load methods
void SettingsManager::loadGeneral()
{
    m_settings.beginGroup("General");
    m_general.defaultUnit = static_cast<GeneralSettings::Unit>(
        m_settings.value("unit", static_cast<int>(GeneralSettings::Millimeters)).toInt());
    m_general.theme = static_cast<GeneralSettings::Theme>(
        m_settings.value("theme", static_cast<int>(GeneralSettings::System)).toInt());
    m_general.showWelcomeScreen = m_settings.value("showWelcomeScreen", false).toBool();
    m_general.checkForUpdates = m_settings.value("checkForUpdates", true).toBool();
    m_settings.endGroup();
}

void SettingsManager::loadEditor()
{
    m_settings.beginGroup("Editor");
    m_editor.gridSpacing = m_settings.value("gridSpacing", 10.0).toDouble();
    m_editor.gridAutoScale = m_settings.value("gridAutoScale", true).toBool();
    m_editor.gridColor = m_settings.value("gridColor", QColor(220, 220, 220)).value<QColor>();
    m_editor.snapDistance = m_settings.value("snapDistance", 5.0).toDouble();
    m_editor.snapToGrid = m_settings.value("snapToGrid", false).toBool();
    m_editor.snapToObjects = m_settings.value("snapToObjects", true).toBool();
    m_editor.selectionColor = m_settings.value("selectionColor", QColor(0, 120, 215)).value<QColor>();
    m_editor.selectionLineWidth = m_settings.value("selectionLineWidth", 2.0).toDouble();
    m_editor.canvasBackgroundColor = m_settings.value("canvasBackgroundColor", QColor(Qt::white)).value<QColor>();
    m_editor.antiAliasing = m_settings.value("antiAliasing", true).toBool();
    m_settings.endGroup();
}

void SettingsManager::loadFileIO()
{
    m_settings.beginGroup("FileIO");
    m_fileIO.autoSaveEnabled = m_settings.value("autoSaveEnabled", true).toBool();
    m_fileIO.autoSaveInterval = m_settings.value("autoSaveInterval", 5).toInt();
    m_fileIO.autoSaveVersions = m_settings.value("autoSaveVersions", 10).toInt();
    m_fileIO.autoSaveLocation = m_settings.value("autoSaveLocation", 0).toInt();
    m_fileIO.autoSaveCustomDirectory = m_settings.value("autoSaveCustomDirectory", "").toString();
    m_fileIO.recentFilesCount = m_settings.value("recentFilesCount", 10).toInt();
    m_fileIO.lastOpenDirectory = m_settings.value("lastOpenDirectory", "").toString();
    m_fileIO.lastSaveDirectory = m_settings.value("lastSaveDirectory", "").toString();
    m_fileIO.lastExportDirectory = m_settings.value("lastExportDirectory", "").toString();
    m_fileIO.compressNativeFormat = m_settings.value("compressNativeFormat", true).toBool();
    m_settings.endGroup();
}

void SettingsManager::loadAdvanced()
{
    m_settings.beginGroup("Advanced");
    m_advanced.undoHistoryMemoryLimit = m_settings.value("undoHistoryMemoryLimit", 200).toInt();
    m_advanced.useOpenGLRendering = m_settings.value("useOpenGLRendering", true).toBool();
    m_advanced.maxRenderThreads = m_settings.value("maxRenderThreads", 4).toInt();
    m_advanced.showFPS = m_settings.value("showFPS", false).toBool();
    m_advanced.logDebugMessages = m_settings.value("logDebugMessages", false).toBool();
    m_settings.endGroup();
}

// Save methods
void SettingsManager::saveGeneral()
{
    m_settings.beginGroup("General");
    m_settings.setValue("unit", static_cast<int>(m_general.defaultUnit));
    m_settings.setValue("theme", static_cast<int>(m_general.theme));
    m_settings.setValue("showWelcomeScreen", m_general.showWelcomeScreen);
    m_settings.setValue("checkForUpdates", m_general.checkForUpdates);
    m_settings.endGroup();
}

void SettingsManager::saveEditor()
{
    m_settings.beginGroup("Editor");
    m_settings.setValue("gridSpacing", m_editor.gridSpacing);
    m_settings.setValue("gridAutoScale", m_editor.gridAutoScale);
    m_settings.setValue("gridColor", m_editor.gridColor);
    m_settings.setValue("snapDistance", m_editor.snapDistance);
    m_settings.setValue("snapToGrid", m_editor.snapToGrid);
    m_settings.setValue("snapToObjects", m_editor.snapToObjects);
    m_settings.setValue("selectionColor", m_editor.selectionColor);
    m_settings.setValue("selectionLineWidth", m_editor.selectionLineWidth);
    m_settings.setValue("canvasBackgroundColor", m_editor.canvasBackgroundColor);
    m_settings.setValue("antiAliasing", m_editor.antiAliasing);
    m_settings.endGroup();
}

void SettingsManager::saveFileIO()
{
    m_settings.beginGroup("FileIO");
    m_settings.setValue("autoSaveEnabled", m_fileIO.autoSaveEnabled);
    m_settings.setValue("autoSaveInterval", m_fileIO.autoSaveInterval);
    m_settings.setValue("autoSaveVersions", m_fileIO.autoSaveVersions);
    m_settings.setValue("autoSaveLocation", m_fileIO.autoSaveLocation);
    m_settings.setValue("autoSaveCustomDirectory", m_fileIO.autoSaveCustomDirectory);
    m_settings.setValue("recentFilesCount", m_fileIO.recentFilesCount);
    m_settings.setValue("lastOpenDirectory", m_fileIO.lastOpenDirectory);
    m_settings.setValue("lastSaveDirectory", m_fileIO.lastSaveDirectory);
    m_settings.setValue("lastExportDirectory", m_fileIO.lastExportDirectory);
    m_settings.setValue("compressNativeFormat", m_fileIO.compressNativeFormat);
    m_settings.endGroup();
}

void SettingsManager::saveAdvanced()
{
    m_settings.beginGroup("Advanced");
    m_settings.setValue("undoHistoryMemoryLimit", m_advanced.undoHistoryMemoryLimit);
    m_settings.setValue("useOpenGLRendering", m_advanced.useOpenGLRendering);
    m_settings.setValue("maxRenderThreads", m_advanced.maxRenderThreads);
    m_settings.setValue("showFPS", m_advanced.showFPS);
    m_settings.setValue("logDebugMessages", m_advanced.logDebugMessages);
    m_settings.endGroup();
}

} // namespace PatternCAD
