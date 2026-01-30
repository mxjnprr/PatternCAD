/**
 * SeamAllowanceWidget.cpp
 *
 * Implementation of SeamAllowanceWidget
 */

#include "SeamAllowanceWidget.h"
#include <QFormLayout>
#include <QGroupBox>

namespace PatternCAD {
namespace UI {

SeamAllowanceWidget::SeamAllowanceWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setStyleSheet("QWidget { background-color: #f0f0f0; border: 2px solid #888; border-radius: 5px; }");
}

SeamAllowanceWidget::~SeamAllowanceWidget()
{
}

void SeamAllowanceWidget::setupUi()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(8);

    // Title
    m_titleLabel = new QLabel("Seam Allowance", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    m_layout->addWidget(m_titleLabel);

    // Form layout for inputs
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(6);

    // Width spinner
    m_widthSpinBox = new QDoubleSpinBox(this);
    m_widthSpinBox->setRange(0.0, 100.0);
    m_widthSpinBox->setDecimals(1);
    m_widthSpinBox->setSuffix(" mm");
    m_widthSpinBox->setSingleStep(1.0);
    m_widthSpinBox->setValue(10.0);
    connect(m_widthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SeamAllowanceWidget::onWidthChanged);
    formLayout->addRow("Width:", m_widthSpinBox);

    // Corner type combo
    m_cornerTypeCombo = new QComboBox(this);
    m_cornerTypeCombo->addItem("Miter", static_cast<int>(CornerType::Miter));
    m_cornerTypeCombo->addItem("Round", static_cast<int>(CornerType::Round));
    m_cornerTypeCombo->addItem("Bevel", static_cast<int>(CornerType::Bevel));
    m_cornerTypeCombo->setCurrentIndex(1);  // Round by default
    connect(m_cornerTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SeamAllowanceWidget::onCornerTypeChanged);
    formLayout->addRow("Corner:", m_cornerTypeCombo);

    m_layout->addLayout(formLayout);

    // Instructions
    QLabel* instructions = new QLabel("Click edges to toggle\nEnter: finish | Esc: cancel", this);
    instructions->setStyleSheet("font-size: 9pt; color: #666;");
    instructions->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(instructions);

    setFixedWidth(220);
}

void SeamAllowanceWidget::showAtPosition(const QPoint& globalPos)
{
    move(globalPos);
    show();
    raise();
    m_widthSpinBox->setFocus();
    m_widthSpinBox->selectAll();
}

double SeamAllowanceWidget::width() const
{
    return m_widthSpinBox->value();
}

void SeamAllowanceWidget::setWidth(double width)
{
    m_widthSpinBox->blockSignals(true);
    m_widthSpinBox->setValue(width);
    m_widthSpinBox->blockSignals(false);
}

CornerType SeamAllowanceWidget::cornerType() const
{
    int typeInt = m_cornerTypeCombo->currentData().toInt();
    return static_cast<CornerType>(typeInt);
}

void SeamAllowanceWidget::setCornerType(CornerType type)
{
    m_cornerTypeCombo->blockSignals(true);
    int index = m_cornerTypeCombo->findData(static_cast<int>(type));
    if (index >= 0) {
        m_cornerTypeCombo->setCurrentIndex(index);
    }
    m_cornerTypeCombo->blockSignals(false);
}

void SeamAllowanceWidget::onWidthChanged(double value)
{
    emit widthChanged(value);
}

void SeamAllowanceWidget::onCornerTypeChanged(int index)
{
    Q_UNUSED(index);
    emit cornerTypeChanged(cornerType());
}

} // namespace UI
} // namespace PatternCAD
