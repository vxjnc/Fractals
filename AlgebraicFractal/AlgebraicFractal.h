#pragma once
#include <CL/opencl.hpp>

#include <random>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <QImage>
#include "vec2.cpp"
#include "vec3.cpp"
#include "vec4.cpp"

class AlgebraicFractal : public QObject
{
    Q_OBJECT
public:
    enum Type
    {
        None,
        MandelbrotSet,
        JuliaSet,
        Buddhabrot,
        BurningShip,
        NewtonPools
    };

    AlgebraicFractal();

    double posX() const;
    double posY() const;
    unsigned int maxIter() const;

    double scale() const;

    int shift() const;

    double CReal();
    double CImag();

    void draw(QImage& image) const;
    void drawBuddhabrot(QImage& image) const;
    void drawCPU(QImage& image) const;
    void drawCL(QImage& image) const;

    void plusScreen( double x, double y, int width, int height, double speedZoom);
    void minusScreen(double x, double y, int width, int height, double speedZoom);

    Type type();

    std::array<QRgb, 1024> palette;

public slots:
    void setType(Type newType);
    void setPosX(double newX);
    void setPosY(double newY);
    void setScale(double newScale);
    void setMaxIter(int newMaxIter);
    void setShift(int newShift);
    void setCReal(double newReal);
    void setCImag(double newImag);

signals:
    void typeChanged(Type type);
    void posXChanged(double newX);
    void posYChanged(double newY);
    void scaleChanged(double newScale);
    void maxIterChanged(int newMaxIter);
    void shiftChanged(int newShift);
    void CRealChanged(double newReal);
    void CImagChanged(double newImag);

private:
    cl::Device device;
    cl::Context context;
    cl::Program::Sources sources;
    cl::Program program;
    bool isSupportCL = true;

    Type typeFractal = Type::MandelbrotSet;

    unsigned int maxIterations = 64;
    int shiftPalette = 512;

    vec2<double> ReRange{-2.0, 2.0};
    vec2<double> ImRange{-2.0 * (379.0 / 503.0), 2.0 * (379.0 / 503.0)};
    vec2<double> juliaC{0.285, 0.01};

    mutable bool needUpdateIterations = true;
    mutable std::vector<unsigned int> cacheIter;
    mutable std::vector<vec2<double>> cacheZ;
    mutable std::vector<unsigned int> count;

    void calcMandelbrotSet(int w, int h) const;
    void calcJuliaSet(int w, int h) const;
    void calcBuddhabrot(int w, int h) const;
    void calcBurningShip(int w, int h) const;
    void calcNewtonPools(int w, int h) const;

    void calcMandelbrotSetCPU(int w, int h) const;
    void calcJuliaSetCPU(int w, int h) const;
    void calcBuddhabrotCPU(int w, int h) const;
    void calcBurningShipCPU(int w, int h) const;
    void calcNewtonPoolsCPU(int w, int h) const;

    void calcMandelbrotSetCL(int w, int h) const;
    void calcJuliaSetCL(int w, int h) const;
    void calcBurningShipCL(int w, int h) const;
    void calcNewtonPoolsCL(int w, int h) const;

    QRgb getColor(unsigned int iteration, const vec2<double>& Z) const;
};
