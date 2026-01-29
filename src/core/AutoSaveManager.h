/**
 * AutoSaveManager.h
 *
 * Manages automatic background saving of documents
 */

#ifndef PATTERNCAD_AUTOSAVEMANAGER_H
#define PATTERNCAD_AUTOSAVEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDateTime>

namespace PatternCAD {

class Document;

/**
 * AutoSaveManager handles automatic periodic saving of documents
 * to protect against crashes and data loss.
 *
 * Features:
 * - Periodic auto-save with configurable interval
 * - Only saves if document has unsaved changes
 * - Creates timestamped .autosave files
 * - Maintains FIFO list of recent auto-saves
 * - Non-blocking saves
 * - Status notifications
 */
class AutoSaveManager : public QObject
{
    Q_OBJECT

public:
    explicit AutoSaveManager(QObject* parent = nullptr);
    ~AutoSaveManager();

    // Configuration
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    void setInterval(int minutes);
    int interval() const { return m_intervalMinutes; }

    void setMaxAutoSaves(int count);
    int maxAutoSaves() const { return m_maxAutoSaves; }

    void setAutoSaveDirectory(const QString& directory);
    QString autoSaveDirectory() const { return m_autoSaveDirectory; }

    // Document management
    void setDocument(Document* document);
    Document* document() const { return m_document; }

    // File path (for auto-save naming)
    void setFilePath(const QString& filePath);
    QString filePath() const { return m_filePath; }

    // Manual trigger
    void triggerAutoSave();

    // Utility
    QStringList findAutoSaveFiles(const QString& baseFilePath) const;
    void cleanupOldAutoSaves(const QString& baseFilePath);
    QString generateAutoSaveFilePath(const QString& baseFilePath) const;

    // Recovery
    static QStringList findAllAutoSaveFiles(const QString& directory);

signals:
    void autoSaveStarted();
    void autoSaveCompleted(const QString& filePath);
    void autoSaveFailed(const QString& error);

private slots:
    void onTimerTimeout();

private:
    bool shouldAutoSave() const;
    void performAutoSave();
    void cleanupOldAutoSaves();

    Document* m_document;
    QTimer* m_timer;

    bool m_enabled;
    int m_intervalMinutes;
    int m_maxAutoSaves;
    QString m_autoSaveDirectory;
    QString m_filePath;

    bool m_saveInProgress;
    QDateTime m_lastAutoSave;
};

} // namespace PatternCAD

#endif // PATTERNCAD_AUTOSAVEMANAGER_H
