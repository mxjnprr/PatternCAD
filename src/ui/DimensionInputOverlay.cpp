/**
 * DimensionInputOverlay.cpp
 *
 * Implementation of floating dimension input overlay
 */

#include "DimensionInputOverlay.h"
#include "core/Units.h"
#include "core/Project.h"
#include <QKeyEvent>
#include <QDoubleValidator>
#include <QApplication>
#include <QScreen>
#include <QDebug>

namespace PatternCAD {
namespace UI {

DimensionInputOverlay::DimensionInputOverlay(QWidget* parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_isAngleMode(false)
    , m_isScaleMode(false)
{
    setupUI();
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_DeleteOnClose, false);
}

DimensionInputOverlay::~DimensionInputOverlay()
{
}

void DimensionInputOverlay::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(8);

    // Prompt label
    m_promptLabel = new QLabel("Length:", this);
    QFont labelFont = m_promptLabel->font();
    labelFont.setBold(true);
    labelFont.setPointSize(10);
    m_promptLabel->setFont(labelFont);
    m_layout->addWidget(m_promptLabel);

    // Input field
    m_input = new QLineEdit(this);
    m_input->setValidator(new QDoubleValidator(0.0, 100000.0, 3, this));
    m_input->setPlaceholderText("Enter value...");
    m_input->setMinimumWidth(150);
    QFont inputFont = m_input->font();
    inputFont.setPointSize(12);
    m_input->setFont(inputFont);
    m_layout->addWidget(m_input);

    // Angle label and input (initially hidden)
    m_angleLabel = new QLabel("Angle (°):", this);
    QFont angleLabelFont = m_angleLabel->font();
    angleLabelFont.setBold(true);
    angleLabelFont.setPointSize(10);
    m_angleLabel->setFont(angleLabelFont);
    m_angleLabel->hide();
    m_layout->addWidget(m_angleLabel);

    m_angleInput = new QLineEdit(this);
    m_angleInput->setValidator(new QDoubleValidator(-360.0, 360.0, 2, this));
    m_angleInput->setPlaceholderText("0.0");
    m_angleInput->setMinimumWidth(150);
    m_angleInput->setFont(inputFont);
    m_angleInput->hide();
    m_layout->addWidget(m_angleInput);

    // Resize mode label and combo (initially hidden)
    m_resizeModeLabel = new QLabel("Resize from:", this);
    QFont modeLabelFont = m_resizeModeLabel->font();
    modeLabelFont.setBold(true);
    modeLabelFont.setPointSize(10);
    m_resizeModeLabel->setFont(modeLabelFont);
    m_resizeModeLabel->hide();
    m_layout->addWidget(m_resizeModeLabel);

    m_resizeModeCombo = new QComboBox(this);
    m_resizeModeCombo->addItem("Fix start point", static_cast<int>(ResizeMode::FixStart));
    m_resizeModeCombo->addItem("Fix end point", static_cast<int>(ResizeMode::FixEnd));
    m_resizeModeCombo->addItem("Center (both sides)", static_cast<int>(ResizeMode::Center));
    m_resizeModeCombo->setCurrentIndex(0);
    m_resizeModeCombo->hide();
    m_layout->addWidget(m_resizeModeCombo);

    // Buttons
    m_buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    // Don't set OK as default button - let QLineEdit's returnPressed handle Enter key
    // m_okButton->setDefault(true);
    m_buttonLayout->addWidget(m_okButton);
    m_buttonLayout->addWidget(m_cancelButton);
    m_layout->addLayout(m_buttonLayout);

    setLayout(m_layout);

    // Styling
    setStyleSheet(
        "QWidget { "
        "  background-color: #F0F0F0; "
        "  border: 2px solid #0066CC; "
        "  border-radius: 6px; "
        "}"
        "QLineEdit { "
        "  background-color: white; "
        "  border: 1px solid #CCCCCC; "
        "  border-radius: 3px; "
        "  padding: 5px; "
        "}"
        "QPushButton { "
        "  background-color: #0066CC; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 3px; "
        "  padding: 5px 15px; "
        "  font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "  background-color: #0052A3; "
        "}"
        "QPushButton:pressed { "
        "  background-color: #003D7A; "
        "}"
    );

    // Set tab order
    setTabOrder(m_input, m_angleInput);
    setTabOrder(m_angleInput, m_resizeModeCombo);
    setTabOrder(m_resizeModeCombo, m_okButton);
    setTabOrder(m_okButton, m_cancelButton);

    // Connect signals
    connect(m_okButton, &QPushButton::clicked, this, &DimensionInputOverlay::onAccept);
    connect(m_cancelButton, &QPushButton::clicked, this, &DimensionInputOverlay::onCancel);
    connect(m_input, &QLineEdit::returnPressed, this, &DimensionInputOverlay::onAccept);
    connect(m_angleInput, &QLineEdit::returnPressed, this, &DimensionInputOverlay::onAccept);

    // Connect combo change to update angle field visibility for scale mode
    connect(m_resizeModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DimensionInputOverlay::onScaleModeChanged);
}

void DimensionInputOverlay::showAtPosition(const QPoint& globalPos, const QString& prompt,
                                           bool withAngle, double initialLength, double initialAngle,
                                           bool withResizeMode, bool isAngleMode, bool isScaleMode)
{
    m_isAngleMode = (isAngleMode || isScaleMode);  // Both don't need unit conversion
    m_isScaleMode = isScaleMode;  // Store for scale mode handling
    m_promptLabel->setText(prompt);

    // Pre-fill with initial values if provided
    if (initialLength != 0.0) {
        if (isAngleMode || isScaleMode) {
            // For angle or scale mode, no conversion needed - use raw value
            m_input->setText(QString::number(initialLength, 'f', 1));
        } else {
            // Convert from internal (mm) to display units (cm)
            double lengthInCm = Units::fromInternal(initialLength, Unit::Centimeters);
            m_input->setText(QString::number(lengthInCm, 'f', 2));
        }
    } else {
        m_input->clear();
    }

    if (initialAngle != 0.0) {
        m_angleInput->setText(QString::number(initialAngle, 'f', 1));
    } else {
        m_angleInput->clear();
    }

    // Show/hide resize mode combo - DO THIS FIRST for scale mode
    if (withResizeMode || isScaleMode) {
        // Clear and populate combo based on mode
        m_resizeModeCombo->clear();

        if (isScaleMode) {
            m_resizeModeLabel->setText("Scale mode:");
            m_resizeModeCombo->addItem("Uniform (proportional)", 1);
            m_resizeModeCombo->addItem("Non-uniform (free)", 0);
            m_resizeModeCombo->setCurrentIndex(0);  // Default to uniform
        } else if (withResizeMode) {
            m_resizeModeLabel->setText("Resize from:");
            m_resizeModeCombo->addItem("Fix start point", static_cast<int>(ResizeMode::FixStart));
            m_resizeModeCombo->addItem("Fix end point", static_cast<int>(ResizeMode::FixEnd));
            m_resizeModeCombo->addItem("Center (both sides)", static_cast<int>(ResizeMode::Center));
            m_resizeModeCombo->setCurrentIndex(0);
        }

        m_resizeModeLabel->show();
        m_resizeModeCombo->show();
    } else {
        m_resizeModeLabel->hide();
        m_resizeModeCombo->hide();
    }

    // Show/hide angle fields (or Y field for scale) - NOW check combo value
    if (withAngle) {
        m_angleLabel->setText("Angle (°):");
        m_angleLabel->show();
        m_angleInput->show();
    } else if (isScaleMode) {
        // For scale mode, show Y field if non-uniform is selected
        // Check current combo selection (now properly initialized)
        bool isNonUniform = (m_resizeModeCombo->currentData().toInt() == 0);
        if (isNonUniform) {
            m_promptLabel->setText("Scale X (%):");
            m_angleLabel->setText("Scale Y (%):");
            m_angleLabel->show();
            m_angleInput->show();
            if (initialAngle != 0.0) {
                m_angleInput->setText(QString::number(initialAngle, 'f', 1));
            } else {
                m_angleInput->setText(QString::number(initialLength, 'f', 1));  // Same as X by default
            }
        } else {
            m_promptLabel->setText("Scale (%):");
            m_angleLabel->hide();
            m_angleInput->hide();
        }
    } else {
        m_angleLabel->hide();
        m_angleInput->hide();
    }

    // Adjust size to content
    adjustSize();

    // Position near the given point, but ensure it's visible on screen
    QPoint pos = globalPos + QPoint(10, 10);

    // Get screen geometry
    QScreen* screen = QApplication::screenAt(globalPos);
    if (!screen) {
        screen = QApplication::primaryScreen();
    }
    QRect screenGeometry = screen->availableGeometry();

    // Adjust if would go off-screen
    if (pos.x() + width() > screenGeometry.right()) {
        pos.setX(screenGeometry.right() - width() - 10);
    }
    if (pos.y() + height() > screenGeometry.bottom()) {
        pos.setY(screenGeometry.bottom() - height() - 10);
    }
    if (pos.x() < screenGeometry.left()) {
        pos.setX(screenGeometry.left() + 10);
    }
    if (pos.y() < screenGeometry.top()) {
        pos.setY(screenGeometry.top() + 10);
    }

    move(pos);
    show();
    raise();
    activateWindow();

    // Set focus and select text AFTER showing the widget
    // This ensures the widget has proper focus for Enter key handling
    m_input->setFocus();
    m_input->selectAll();
}

double DimensionInputOverlay::getValue() const
{
    bool ok;
    double value = m_input->text().toDouble(&ok);
    if (!ok) {
        return 0.0;
    }

    // For angle mode, return raw value without unit conversion
    if (m_isAngleMode) {
        return value;
    }

    // Convert from current project units to internal (mm)
    // For now, assume centimeters (TODO: get from project settings)
    return Units::toInternal(value, Unit::Centimeters);
}

double DimensionInputOverlay::getAngle() const
{
    bool ok;
    double angle = m_angleInput->text().toDouble(&ok);
    if (!ok) {
        return 0.0;
    }
    return angle;
}

bool DimensionInputOverlay::hasAngle() const
{
    return m_angleInput->isVisible() && !m_angleInput->text().isEmpty();
}

ResizeMode DimensionInputOverlay::getResizeMode() const
{
    if (m_resizeModeCombo->isVisible()) {
        return static_cast<ResizeMode>(m_resizeModeCombo->currentData().toInt());
    }
    return ResizeMode::FixStart;  // Default
}

bool DimensionInputOverlay::isUniformScale() const
{
    if (m_resizeModeCombo->isVisible()) {
        // For scale mode: 1 = uniform, 0 = non-uniform
        return m_resizeModeCombo->currentData().toInt() == 1;
    }
    return true;  // Default to uniform
}

void DimensionInputOverlay::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        onCancel();
        event->accept();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Handle Enter/Return explicitly to ensure it works even when button has focus
        onAccept();
        event->accept();
    } else if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab) {
        // Let Tab work normally for field navigation, don't propagate to parent
        QWidget::keyPressEvent(event);
    } else {
        QWidget::keyPressEvent(event);
    }
}

void DimensionInputOverlay::onAccept()
{
    double value = getValue();
    double angle = getAngle();
    ResizeMode mode = getResizeMode();
    bool uniform = isUniformScale();  // Get this BEFORE hide()
    qDebug() << "DimensionInputOverlay::onAccept - value=" << value << "angle=" << angle << "uniform=" << uniform;
    hide();
    emit valueAccepted(value, angle, mode, uniform);  // Pass uniform in signal
}

void DimensionInputOverlay::onCancel()
{
    hide();
    emit valueCancelled();
}

void DimensionInputOverlay::onScaleModeChanged(int index)
{
    Q_UNUSED(index);

    if (!m_isScaleMode) {
        return;  // Only handle this for scale mode
    }

    // Check if uniform (1) or non-uniform (0)
    bool isNonUniform = (m_resizeModeCombo->currentData().toInt() == 0);

    if (isNonUniform) {
        // Show Y field
        m_promptLabel->setText("Scale X (%):");
        m_angleLabel->setText("Scale Y (%):");
        m_angleLabel->show();
        m_angleInput->show();

        // Copy X value to Y if Y is empty
        if (m_angleInput->text().isEmpty() && !m_input->text().isEmpty()) {
            m_angleInput->setText(m_input->text());
        }
    } else {
        // Hide Y field
        m_promptLabel->setText("Scale (%):");
        m_angleLabel->hide();
        m_angleInput->hide();
    }

    // Adjust dialog size
    adjustSize();
}

} // namespace UI
} // namespace PatternCAD
