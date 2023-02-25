#pragma once

#include <QImage>
#include <QColor>

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <map>
#include <string>
#include "vec2.cpp"

class StochasticFractal : public QObject
{
    Q_OBJECT;

public:
    struct coefficients
    {
        coefficients(double a = 0.0, double b = 0.0, double c = 0.0, double d = 0.0, double e = 0.0, double f = 0.0, double p = 0.0, int t = 0) : a(a), b(b), c(c), d(d), e(e), f(f), p(p), type(t)
        {
        }

        double a;
        double b;
        double c;
        double d;
        double e;
        double f;
        double p;
        int type;
    };
    using transformations = std::vector<coefficients>;

    enum Type
    {
        None,
        IFS
    };

    StochasticFractal();

    void drawPoints(QImage& image, const QString& fractal);

    Type type() const;
    const std::map<QString, transformations>& map() const;
    double scaleFactor() const;
    int countIter() const;

    const transformations& transformation(const QString& fractal);

public slots:
    void setType(Type newType);
    void setMap(const std::map<QString, transformations>& newMap);
    void setScaleFactor(double newScaleFactor);
    void setCountIter(int newCountIter);

signals:
    void typeChanged(Type type);
    void mapChanged(const std::map<QString, transformations>& map);
    void scaleFactorChanged(double scaleFactor);
    void countIterChanged(int countIter);

private:
    Type typeFractal = Type::IFS;
    std::map<QString, transformations> m;

    double scaleFctr = 1.0;
    int countIterFrame = 10000;

    std::mt19937 gen;
    vec2<double> p;

    template <typename T>
    constexpr vec2<T> f(T x, T y, const coefficients& c) const;
};
