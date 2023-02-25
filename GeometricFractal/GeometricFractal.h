#pragma once

#include <QImage>
#include <QColor>
#include <QPainter>

#include <QStringView>
#include <QString>
#include <algorithm>
#include <sstream>
#include <stack>
#include <map>

class GeometricFractal : public QObject
{
    Q_OBJECT;
public:
    enum Type
    {
        None,
        LSystem
    };

    GeometricFractal();

    const QString& axiom() const;
    QString getStringRules() const;
    int iterations() const;
    int length() const;
    int rAngle() const;
    int lAngle() const;

    void draw(QImage& image, const QColor& color = Qt::white) const;

    QPoint offset{(int)(503 * 0.25), (int)(379 * 0.25)};

public slots:
    void setAxiom(const QString& axiom);
    void setRules(const QString& strRules);
    void setIterations(int iterations);
    void setRAngle(int rangle);
    void setLAngle(int langle);
    void setLength(int lenght);

signals:
    void axiomChanged(const QString& newAxiom);
    void rulesChanged(const QString& newRules);
    void iterationsChanged(int newIter);
    void lengthChanged(int newLenght);
    void rAngleChanged(int newRAngle);
    void lAngleChanged(int newLAngle);

private:
    int rangle = 60;
    int langle = 60;

    int _length = 1;

    QString _axiom = "F--F--F";
    QString end;

    int iters = 5;
    std::map<QChar, QString> rules
    {
        {'F', "F+F--F+F"},
        {'+', "+"},
        {'-', "-"},
        {'[', "["},
        {']', "]"}
    };

    void updateEnd();
};
