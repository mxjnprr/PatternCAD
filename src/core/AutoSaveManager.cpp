/**
 * AutoSaveManager.cpp
 *
 * Implementation of AutoSaveManager
 */

#include "AutoSaveManager.h"
#include "Document.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

namespace PatternCAD {

AutoSaveManager::AutoSaveManager(QObject* parent)
    : QObject(parent)
    , m_document(nullptr)
    , m_timer(new QTimer(this))
    , m_enabled(true)
    , m_intervalMinutes(5)
    , m_maxAutoSaves(10)
    , m_autoSaveDirectory()
    , m_saveInProgress(false)
{
    // Setup timer
    m_timer->setTimerType(Qt::VeryCoarseTimer); // Save power
    connect(m_timer, &QTimer::timeout, this, &AutoSaveManager::onTimerTimeout);

    // Default auto-save directory: system temp
    m_autoSaveDirectory = QDir::tempPath() + "/patterncad-autosave";
    QDir().mkpath(m_autoSaveDirectory);

    // Start timer if enabled
    if (m_enabled) {
        m_timer->start(m_intervalMinutes * 60 * 1000); // minutes to milliseconds
    }
}

AutoSaveManager::~AutoSaveManager()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void AutoSaveManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;

    if (m_enabled) {
        m_timer->start(m_intervalMinutes * 60 * 1000);
        qDebug() << "Auto-save enabled with interval:" << m_intervalMinutes << "minutes";
    } else {
        m_timer->stop();
        qDebug() << "Auto-save disabled";
    }
}

void AutoSaveManager::setInterval(int minutes)
{
    if (minutes < 1) minutes = 1;
    if (minutes > 60) minutes = 60;

    m_intervalMinutes = minutes;

    if (m_enabled && m_timer->isActive()) {
        m_timer->setInterval(m_intervalMinutes * 60 * 1000);
        qDebug() << "Auto-save interval changed to:" << m_intervalMinutes << "minutes";
    }
}

void AutoSaveManager::setMaxAutoSaves(int count)
{
    if (count < 1) count = 1;
    if (count > 50) count = 50;

    m_maxAutoSaves = count;
    qDebug() << "Max auto-saves set to:" << m_maxAutoSaves;
}

void AutoSaveManager::setAutoSaveDirectory(const QString& directory)
{
    m_autoSaveDirectory = directory;
    QDir().mkpath(m_autoSaveDirectory);
    qDebug() << "Auto-save directory set to:" << m_autoSaveDirectory;
}

void AutoSaveManager::setDocument(Document* document)
{
    m_document = document;
}

void AutoSaveManager::setFilePath(const QString& filePath)
{
    m_filePath = filePath;
}

void AutoSaveManager::triggerAutoSave()
{
    if (m_saveInProgress) {
        qDebug() << "Auto-save already in progress, skipping trigger";
        return;
    }

    if (shouldAutoSave()) {
        performAutoSave();
    }
}

void AutoSaveManager::onTimerTimeout()
{
    if (!m_enabled) {
        return;
    }

    triggerAutoSave();
}

bool AutoSaveManager::shouldAutoSave() const
{
    if (!m_document) {
        return false;
    }

    if (!m_document->isModified()) {
        return false;
    }

    // Don't auto-save too frequently (prevent rapid successive saves)
    if (m_lastAutoSave.isValid()) {
        qint64 secondsSinceLastSave = m_lastAutoSave.secsTo(QDateTime::currentDateTime());
        if (secondsSinceLastSave < 30) { // Minimum 30 seconds between auto-saves
            return false;
        }
    }

    return true;
}

void AutoSaveManager::performAutoSave()
{
    if (!m_document) {
        return;
    }

    m_saveInProgress = true;
    emit autoSaveStarted();

    // Generate auto-save file path
    QString baseFilePath = m_filePath;
    if (baseFilePath.isEmpty()) {
        baseFilePath = m_autoSaveDirectory + "/untitled";
    }

    QString autoSaveFilePath = generateAutoSaveFilePath(baseFilePath);

    qDebug() << "Performing auto-save to:" << autoSaveFilePath;

    // Perform save
    bool success = m_document->save(autoSaveFilePath);

    m_saveInProgress = false;
    m_lastAutoSave = QDateTime::currentDateTime();

    if (success) {
        emit autoSaveCompleted(autoSaveFilePath);
        qDebug() << "Auto-save completed successfully";

        // Cleanup old auto-saves
        cleanupOldAutoSaves(baseFilePath);
    } else {
        emit autoSaveFailed(tr("Failed to write auto-save file"));
        qDebug() << "Auto-save failed";
    }
}

QString AutoSaveManager::generateAutoSaveFilePath(const QString& baseFilePath)
{
    QFileInfo fileInfo(baseFilePath);
    QString baseName = fileInfo.completeBaseName(); // Without extension
    QString directory = fileInfo.absolutePath();

    // If base path is just a name (no directory), use auto-save directory
    if (directory == "." || baseName == baseFilePath) {
        directory = QDir::tempPath() + "/patterncad-autosave";
        QDir().mkpath(directory);
    }

    // Generate timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");

    // Build auto-save path
    QString autoSaveFilePath = QString("%1/%2-%3.autosave")
                                   .arg(directory)
                                   .arg(baseName)
                                   .arg(timestamp);

    return autoSaveFilePath;
}

QStringList AutoSaveManager::findAutoSaveFiles(const QString& baseFilePath) const
{
    QFileInfo fileInfo(baseFilePath);
    QString baseName = fileInfo.completeBaseName();
    QString directory = fileInfo.absolutePath();

    // If no directory, check auto-save directory
    if (directory == "." || baseName == baseFilePath) {
        directory = m_autoSaveDirectory;
    }

    QDir dir(directory);
    if (!dir.exists()) {
        return QStringList();
    }

    // Find all auto-save files for this base name
    QString pattern = QString("%1-*.autosave").arg(baseName);
    QStringList filters;
    filters << pattern;

    QStringList autoSaveFiles = dir.entryList(filters, QDir::Files, QDir::Time);

    // Convert to absolute paths
    QStringList absolutePaths;
    for (const QString& fileName : autoSaveFiles) {
        absolutePaths << dir.absoluteFilePath(fileName);
    }

    return absolutePaths;
}

void AutoSaveManager::cleanupOldAutoSaves(const QString& baseFilePath)
{
    QStringList autoSaveFiles = findAutoSaveFiles(baseFilePath);

    // Keep only the most recent N files
    while (autoSaveFiles.size() > m_maxAutoSaves) {
        QString oldestFile = autoSaveFiles.takeLast(); // Last = oldest (sorted by time)
        QFile::remove(oldestFile);
        qDebug() << "Removed old auto-save:" << oldestFile;
    }
}

void AutoSaveManager::cleanupOldAutoSaves()
{
    QString baseFilePath = m_filePath;
    if (baseFilePath.isEmpty()) {
        baseFilePath = m_autoSaveDirectory + "/untitled";
    }

    cleanupOldAutoSaves(baseFilePath);
}

} // namespace PatternCAD
