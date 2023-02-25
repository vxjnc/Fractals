#include "DoubleRoundSpinBox.h"

DoubleRoundSpinBox::DoubleRoundSpinBox(QWidget* parent) : QDoubleSpinBox(parent)
{

}

QString DoubleRoundSpinBox::textFromValue(double value) const
{
    char* str = new char[30];
    snprintf(str, 30, ("%." + std::to_string(decimals()) + "g").data(), value);

    QString qstr(str);
    delete[] str;

    return qstr;
}
