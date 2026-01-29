/**
 * DimensionInputWidget.cpp
 *
 * Implementation of DimensionInputWidget
 */

#include "DimensionInputWidget.h"
#include "core/Units.h"
#include <QKeyEvent>
#include <QDoubleValidator>

namespace PatternCAD {
namespace UI {

DimensionInputWidget::DimensionInputWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    hide();
}

void DimensionInputWidget::setupUI()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_layout->setSpacing(10);

    // Create labels and input fields
    m_xLabel = new QLabel("X:", this);
    m_xInput = new QLineEdit(this);
    m_xInput->setMaximumWidth(100);
    m_xInput->setValidator(new QDoubleValidator(-100000, 100000, 3, this));
    m_xInput->setPlaceholderText("0.0");

    m_yLabel = new QLabel("Y:", this);
    m_yInput = new QLineEdit(this);
    m_yInput->setMaximumWidth(100);
    m_yInput->setValidator(new QDoubleValidator(-100000, 100000, 3, this));
    m_yInput->setPlaceholderText("0.0");

    m_lengthLabel = new QLabel("Length:", this);
    m_lengthInput = new QLineEdit(this);
    m_lengthInput->setMaximumWidth(100);
    m_lengthInput->setValidator(new QDoubleValidator(0, 100000, 3, this));
    m_lengthInput->setPlaceholderText("0.0");

    m_widthLabel = new QLabel("Width:", this);
    m_widthInput = new QLineEdit(this);
    m_widthInput->setMaximumWidth(100);
    m_widthInput->setValidator(new QDoubleValidator(0, 100000, 3, this));
    m_widthInput->setPlaceholderText("0.0");

    m_heightLabel = new QLabel("Height:", this);
    m_heightInput = new QLineEdit(this);
    m_heightInput->setMaximumWidth(100);
    m_heightInput->setValidator(new QDoubleValidator(0, 100000, 3, this));
    m_heightInput->setPlaceholderText("0.0");

    m_radiusLabel = new QLabel("Radius:", this);
    m_radiusInput = new QLineEdit(this);
    m_radiusInput->setMaximumWidth(100);
    m_radiusInput->setValidator(new QDoubleValidator(0, 100000, 3, this));
    m_radiusInput->setPlaceholderText("0.0");

    // Add all widgets to layout (they'll be shown/hidden as needed)
    m_layout->addWidget(m_xLabel);
    m_layout->addWidget(m_xInput);
    m_layout->addWidget(m_yLabel);
    m_layout->addWidget(m_yInput);
    m_layout->addWidget(m_lengthLabel);
    m_layout->addWidget(m_lengthInput);
    m_layout->addWidget(m_widthLabel);
    m_layout->addWidget(m_widthInput);
    m_layout->addWidget(m_heightLabel);
    m_layout->addWidget(m_heightInput);
    m_layout->addWidget(m_radiusLabel);
    m_layout->addWidget(m_radiusInput);
    m_layout->addStretch();

    setLayout(m_layout);
    hideAllFields();
}

void DimensionInputWidget::setCoordinateMode(bool relative)
{
    hideAllFields();

    if (relative) {
        m_xLabel->setText("ΔX:");
        m_yLabel->setText("ΔY:");
    } else {
        m_xLabel->setText("X:");
        m_yLabel->setText("Y:");
    }

    m_xLabel->show();
    m_xInput->show();
    m_yLabel->show();
    m_yInput->show();
}

void DimensionInputWidget::setLengthMode()
{
    hideAllFields();
    m_lengthLabel->show();
    m_lengthInput->show();
}

void DimensionInputWidget::setRectangleMode()
{
    hideAllFields();
    m_widthLabel->show();
    m_widthInput->show();
    m_heightLabel->show();
    m_heightInput->show();
}

void DimensionInputWidget::setCircleMode()
{
    hideAllFields();
    m_radiusLabel->show();
    m_radiusInput->show();
}

void DimensionInputWidget::reset()
{
    m_xInput->clear();
    m_yInput->clear();
    m_lengthInput->clear();
    m_widthInput->clear();
    m_heightInput->clear();
    m_radiusInput->clear();
}

void DimensionInputWidget::hideAllFields()
{
    m_xLabel->hide();
    m_xInput->hide();
    m_yLabel->hide();
    m_yInput->hide();
    m_lengthLabel->hide();
    m_lengthInput->hide();
    m_widthLabel->hide();
    m_widthInput->hide();
    m_heightLabel->hide();
    m_heightInput->hide();
    m_radiusLabel->hide();
    m_radiusInput->hide();
}

double DimensionInputWidget::parseValue(const QString& text) const
{
    if (text.isEmpty()) {
        return 0.0;
    }

    // Convert from current unit to internal (mm)
    double value = text.toDouble();
    return Units::fromCurrentUnit(value);
}

double DimensionInputWidget::getX() const
{
    return parseValue(m_xInput->text());
}

double DimensionInputWidget::getY() const
{
    return parseValue(m_yInput->text());
}

double DimensionInputWidget::getLength() const
{
    return parseValue(m_lengthInput->text());
}

double DimensionInputWidget::getWidth() const
{
    return parseValue(m_widthInput->text());
}

double DimensionInputWidget::getHeight() const
{
    return parseValue(m_heightInput->text());
}

double DimensionInputWidget::getRadius() const
{
    return parseValue(m_radiusInput->text());
}

void DimensionInputWidget::setX(double value)
{
    m_xInput->setText(QString::number(Units::toCurrentUnit(value), 'f', 2));
}

void DimensionInputWidget::setY(double value)
{
    m_yInput->setText(QString::number(Units::toCurrentUnit(value), 'f', 2));
}

void DimensionInputWidget::setLength(double value)
{
    m_lengthInput->setText(QString::number(Units::toCurrentUnit(value), 'f', 2));
}

void DimensionInputWidget::setWidth(double value)
{
    m_widthInput->setText(QString::number(Units::toCurrentUnit(value), 'f', 2));
}

void DimensionInputWidget::setHeight(double value)
{
    m_heightInput->setText(QString::number(Units::toCurrentUnit(value), 'f', 2));
}

void DimensionInputWidget::setRadius(double value)
{
    m_radiusInput->setText(QString::number(Units::toCurrentUnit(value), 'f', 2));
}

void DimensionInputWidget::show()
{
    QWidget::show();
}

void DimensionInputWidget::hide()
{
    QWidget::hide();
}

void DimensionInputWidget::focusFirstField()
{
    if (m_xInput->isVisible()) {
        m_xInput->setFocus();
        m_xInput->selectAll();
    } else if (m_lengthInput->isVisible()) {
        m_lengthInput->setFocus();
        m_lengthInput->selectAll();
    } else if (m_widthInput->isVisible()) {
        m_widthInput->setFocus();
        m_widthInput->selectAll();
    } else if (m_radiusInput->isVisible()) {
        m_radiusInput->setFocus();
        m_radiusInput->selectAll();
    }
}

void DimensionInputWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit valueAccepted();
        event->accept();
    } else if (event->key() == Qt::Key_Escape) {
        emit valueCancelled();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

} // namespace UI
} // namespace PatternCAD
