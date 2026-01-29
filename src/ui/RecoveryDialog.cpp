/**
 * RecoveryDialog.cpp
 *
 * Implementation of RecoveryDialog
 */

#include "RecoveryDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>

namespace PatternCAD {
namespace UI {

RecoveryDialog::RecoveryDialog(const QStringList& autoSaveFiles, QWidget* parent)
    : QDialog(parent)
    , m_tableWidget(nullptr)
    , m_recoverButton(nullptr)
    , m_discardButton(nullptr)
    , m_cancelButton(nullptr)
    , m_selectedFile()
    , m_deleteAutoSaves(false)
    , m_autoSaveFiles(autoSaveFiles)
{
    setupUi();
    populateTable(autoSaveFiles);
}

RecoveryDialog::~RecoveryDialog()
{
}

void RecoveryDialog::setupUi()
{
    setWindowTitle(tr("Recover Auto-Saved Files"));
    setMinimumSize(700, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Message
    QLabel* messageLabel = new QLabel(this);
    messageLabel->setText(tr("PatternCAD found auto-saved files from a previous session.\n"
                             "Would you like to recover your work?"));
    messageLabel->setWordWrap(true);
    mainLayout->addWidget(messageLabel);

    // Table
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(4);
    m_tableWidget->setHorizontalHeaderLabels(QStringList()
        << tr("Filename")
        << tr("Saved")
        << tr("Size")
        << tr("Path"));
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_tableWidget, &QTableWidget::itemSelectionChanged,
            this, &RecoveryDialog::onSelectionChanged);
    mainLayout->addWidget(m_tableWidget);

    // Info label
    QLabel* infoLabel = new QLabel(this);
    infoLabel->setText(tr("Select a file and click 'Recover' to restore it.\n"
                          "Click 'Discard All' to delete all auto-save files and start fresh.\n"
                          "Click 'Cancel' to keep the files for later."));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #666; font-size: 10px;");
    mainLayout->addWidget(infoLabel);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_recoverButton = new QPushButton(tr("Recover"), this);
    m_recoverButton->setEnabled(false);
    connect(m_recoverButton, &QPushButton::clicked, this, &RecoveryDialog::onRecover);
    buttonLayout->addWidget(m_recoverButton);

    m_discardButton = new QPushButton(tr("Discard All"), this);
    connect(m_discardButton, &QPushButton::clicked, this, &RecoveryDialog::onDiscard);
    buttonLayout->addWidget(m_discardButton);

    m_cancelButton = new QPushButton(tr("Cancel"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);
}

void RecoveryDialog::populateTable(const QStringList& autoSaveFiles)
{
    m_tableWidget->setRowCount(autoSaveFiles.size());

    for (int i = 0; i < autoSaveFiles.size(); ++i) {
        QFileInfo fileInfo(autoSaveFiles[i]);

        // Filename (without -timestamp.autosave)
        QString fileName = fileInfo.completeBaseName();
        // Remove timestamp part (e.g., "myfile-20260129-143022" -> "myfile")
        int lastDash = fileName.lastIndexOf('-');
        if (lastDash > 0) {
            int secondLastDash = fileName.lastIndexOf('-', lastDash - 1);
            if (secondLastDash > 0) {
                fileName = fileName.left(secondLastDash);
            }
        }
        fileName += ".patterncad";
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(fileName));

        // Timestamp
        QDateTime modified = fileInfo.lastModified();
        QString timeStr = modified.toString("yyyy-MM-dd HH:mm:ss");
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(timeStr));

        // Size
        qint64 sizeBytes = fileInfo.size();
        QString sizeStr;
        if (sizeBytes < 1024) {
            sizeStr = QString("%1 B").arg(sizeBytes);
        } else if (sizeBytes < 1024 * 1024) {
            sizeStr = QString("%1 KB").arg(sizeBytes / 1024.0, 0, 'f', 1);
        } else {
            sizeStr = QString("%1 MB").arg(sizeBytes / (1024.0 * 1024.0), 0, 'f', 2);
        }
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(sizeStr));

        // Full path
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(fileInfo.absoluteFilePath()));
    }

    m_tableWidget->resizeColumnsToContents();

    // Select first row by default
    if (autoSaveFiles.size() > 0) {
        m_tableWidget->selectRow(0);
    }
}

void RecoveryDialog::onRecover()
{
    int selectedRow = m_tableWidget->currentRow();
    if (selectedRow < 0 || selectedRow >= m_autoSaveFiles.size()) {
        QMessageBox::warning(this, tr("No Selection"),
                           tr("Please select a file to recover."));
        return;
    }

    m_selectedFile = m_autoSaveFiles[selectedRow];
    m_deleteAutoSaves = false;

    accept();
}

void RecoveryDialog::onDiscard()
{
    int result = QMessageBox::question(
        this,
        tr("Discard Auto-Saves"),
        tr("Are you sure you want to delete all auto-save files?\n"
           "This action cannot be undone."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (result == QMessageBox::Yes) {
        m_selectedFile = "";
        m_deleteAutoSaves = true;
        accept();
    }
}

void RecoveryDialog::onSelectionChanged()
{
    m_recoverButton->setEnabled(m_tableWidget->currentRow() >= 0);
}

} // namespace UI
} // namespace PatternCAD
