#include "GeometricFractal.h"
#include <QDebug>

GeometricFractal::GeometricFractal()
{
    updateEnd();
}

const QString& GeometricFractal::axiom() const
{
    return _axiom;
}
QString GeometricFractal::getStringRules() const
{
    QString strRules;
    strRules.reserve(rules.size() * 2);

    for (const auto& [key, value] : rules)
    {
        if (key == '+' || key == '-' || key == '[' || key == ']') continue;

        strRules += key;
        strRules += '=';
        strRules += value;
        strRules += ';';
    }

    return strRules;
}
int GeometricFractal::iterations() const
{
    return iters;
}
int GeometricFractal::length() const
{
    return _length;
}
int GeometricFractal::rAngle() const
{
    return rangle;
}
int GeometricFractal::lAngle() const
{
    return langle;
}

void GeometricFractal::setAxiom(const QString& axiom)
{
    _axiom = axiom;
    updateEnd();
    emit axiomChanged(axiom);
}
void GeometricFractal::setRules(const QString& strRules)
{
    QStringList textRules = strRules.split(';');
    QStringList words;

    QString value;

    size_t count = std::count(strRules.begin(), strRules.end(), ';');
    size_t i = 0;
    for (const auto& rule : textRules)
    {
        if (i >= count) break;
        if (rule.size() == 0) continue;

        char key;
        value.clear();

        words = rule.split('=');
        if (words.size() == 2)
        {
            key = words[0].toStdString()[0];
            value = words[1];
            rules[key] = value;
        }

        ++i;
    }

    updateEnd();
    emit rulesChanged(strRules);
}
void GeometricFractal::setIterations(int iterations)
{
    if (iterations < 0) return;

    this->iters = iterations;
    updateEnd();
    emit iterationsChanged(iterations);
}
void GeometricFractal::setLength(int length)
{
    _length = length;
    emit lengthChanged(length);
}
void GeometricFractal::setRAngle(int rangle)
{
    this->rangle = rangle;
    emit rAngleChanged(rangle);
}
void GeometricFractal::setLAngle(int langle)
{
    this->langle = langle;
    emit lAngleChanged(langle);
}

void GeometricFractal::draw(QImage& image, const QColor& color) const
{
    QPainter painter(&image);
    painter.fillRect(0, 0, image.width(), image.height(), Qt::black);

    painter.setPen(color);

    painter.translate(offset);

    std::stack<QTransform> stack;

    for (auto c : end)
    {
        if (c == 'F')
        {
            painter.drawLine(0, 0, 0, _length);
            painter.translate(0, _length);
        }
        else if (c == '+')
        {
            painter.rotate(rangle);
        }
        else if (c == '-')
        {
            painter.rotate(-langle);
        }
        else if (c == '[')
        {
            stack.push(painter.transform());
        }
        else if (c == ']')
        {
            painter.setTransform(stack.top());
            stack.pop();
        }
    }
}

void GeometricFractal::updateEnd()
{
    if (iters == 0)
    {
        end = _axiom;
        return;
    }

    QString result(_axiom.size() * iters, '\0');
    static QString temp(8, ' ');
    temp = _axiom;

    for (int i = 0; i < iters; ++i)
    {
        for (auto c : temp)
        {
            if (rules.count(c) != 0)
            {
                result += rules[c];
            }
        }

        temp = result;
        if (i != iters - 1) result.clear();
    }

    end = result;
}
