/**
 * KeyboardShortcutsDialog.h
 *
 * Dialog to display keyboard shortcuts
 */

#ifndef PATTERNCAD_KEYBOARDSHORTCUTSDIALOG_H
#define PATTERNCAD_KEYBOARDSHORTCUTSDIALOG_H

#include <QDialog>

namespace PatternCAD {
namespace UI {

class KeyboardShortcutsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyboardShortcutsDialog(QWidget* parent = nullptr);
    ~KeyboardShortcutsDialog();

private:
    void setupUI();
    QString getShortcutsHTML() const;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_KEYBOARDSHORTCUTSDIALOG_H
