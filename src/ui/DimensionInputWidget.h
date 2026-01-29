/**
 * DimensionInputWidget.h
 *
 * Widget for entering exact dimensions during object creation
 */

#ifndef PATTERNCAD_DIMENSIONINPUTWIDGET_H
#define PATTERNCAD_DIMENSIONINPUTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

namespace PatternCAD {
namespace UI {

/**
 * DimensionInputWidget provides a panel for entering exact coordinates and dimensions
 * while drawing objects. Similar to CAD coordinate input.
 */
class DimensionInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DimensionInputWidget(QWidget* parent = nullptr);

    // Set labels and enable/disable fields
    void setCoordinateMode(bool relative = false);  // X, Y coordinate input
    void setLengthMode();                           // Length input
    void setRectangleMode();                        // Width, Height input
    void setCircleMode();                           // Radius input
    void reset();

    // Get values (in internal units - millimeters)
    double getX() const;
    double getY() const;
    double getLength() const;
    double getWidth() const;
    double getHeight() const;
    double getRadius() const;

    // Set current values
    void setX(double value);
    void setY(double value);
    void setLength(double value);
    void setWidth(double value);
    void setHeight(double value);
    void setRadius(double value);

    // Show/hide widget
    void show();
    void hide();

    // Focus on first input field
    void focusFirstField();

signals:
    void valueAccepted();  // User pressed Enter
    void valueCancelled(); // User pressed Escape

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    QLineEdit* m_xInput;
    QLineEdit* m_yInput;
    QLineEdit* m_lengthInput;
    QLineEdit* m_widthInput;
    QLineEdit* m_heightInput;
    QLineEdit* m_radiusInput;

    QLabel* m_xLabel;
    QLabel* m_yLabel;
    QLabel* m_lengthLabel;
    QLabel* m_widthLabel;
    QLabel* m_heightLabel;
    QLabel* m_radiusLabel;

    QHBoxLayout* m_layout;

    void setupUI();
    void hideAllFields();
    double parseValue(const QString& text) const;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_DIMENSIONINPUTWIDGET_H
