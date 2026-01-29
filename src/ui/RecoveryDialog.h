/**
 * RecoveryDialog.h
 *
 * Dialog for recovering auto-saved files after crash
 */

#ifndef PATTERNCAD_RECOVERYDIALOG_H
#define PATTERNCAD_RECOVERYDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QList>
#include <QFileInfo>

namespace PatternCAD {
namespace UI {

/**
 * RecoveryDialog displays auto-save files found on startup
 * and allows user to recover or discard them.
 */
class RecoveryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecoveryDialog(const QStringList& autoSaveFiles, QWidget* parent = nullptr);
    ~RecoveryDialog();

    // Get selected file to recover (empty if discarded or cancelled)
    QString selectedFile() const { return m_selectedFile; }

    // Whether to delete auto-save files
    bool shouldDeleteAutoSaves() const { return m_deleteAutoSaves; }

private slots:
    void onRecover();
    void onDiscard();
    void onSelectionChanged();

private:
    void setupUi();
    void populateTable(const QStringList& autoSaveFiles);

    QTableWidget* m_tableWidget;
    QPushButton* m_recoverButton;
    QPushButton* m_discardButton;
    QPushButton* m_cancelButton;

    QString m_selectedFile;
    bool m_deleteAutoSaves;
    QStringList m_autoSaveFiles;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_RECOVERYDIALOG_H
