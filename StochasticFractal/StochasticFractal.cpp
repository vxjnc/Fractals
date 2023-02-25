#include "StochasticFractal.h"

// [From1; From2) -> [To1; To2)
template <typename T> constexpr T norm(double value, double From1, double From2, double To1, double To2)
{
    return (value - From1) / (From2 - From1) * (To2 - To1) + To1;
}
#include <QDebug>
StochasticFractal::StochasticFractal()
{
    gen.seed(std::random_device{}());

    std::ifstream ifs("ifs.txt", std::ios::in);
    if (ifs.bad())
    {
        std::cerr << "ifs.txt not found" << std::endl;
        exit (1);
    }

    QString name;
    int type = 0;
    for (std::string line; std::getline(ifs, line); )
    {
        std::istringstream oss(line);

        if (line.size() == 0) name = "";
        if (name.size() == 0)
        {
            std::string stdname;
            oss >> type;
            oss >> stdname;
            name = stdname.data();
        }
        else
        {
            coefficients c;
            oss >> c.a;
            oss >> c.b;
            oss >> c.c;
            oss >> c.d;
            oss >> c.e;
            oss >> c.f;
            oss >> c.p;
            c.type = type;
            m[name].push_back(c);
        }
    }
}

void StochasticFractal::drawPoints(QImage& image, const QString& fractal)
{
    std::vector<double> weights;
    std::vector<double> indexs;
    weights.reserve(m[fractal].size());

    int i = 0;
    for (auto& cs : m[fractal])
    {
        weights.push_back(cs.p);
        indexs.push_back(i);
        indexs.push_back(i);
        weights.push_back(0.0);
        ++i;
    }
    weights.pop_back();

    std::piecewise_constant_distribution<> dist(indexs.begin(), indexs.end(), weights.begin());
    double scale = image.height() / scaleFctr;

    for (int i = 0; i < countIterFrame; ++i)
    {
        int ind = (int)dist(gen);
        auto c = m[fractal][ind];
        p = f(p.x, p.y, c);

        int x = p.x * scale + image.width() / 2.0;
        int y = image.height() / 2 - (p.y * scale);

        if (x >= 0 && y >= 0 && x < image.width() && y < image.height()) image.setPixelColor(x, y, Qt::white);
    }
}

StochasticFractal::Type StochasticFractal::type() const
{
    return typeFractal;
}
void StochasticFractal::setType(StochasticFractal::Type newType)
{
    typeFractal = newType;
    emit(typeChanged(typeFractal));
}

const std::map<QString, StochasticFractal::transformations>& StochasticFractal::map() const
{
    return m;
}
void StochasticFractal::setMap(const std::map<QString, StochasticFractal::transformations>& newMap)
{
    m = newMap;
    emit(mapChanged(m));
}

double StochasticFractal::scaleFactor() const
{
    return scaleFctr;
}
void StochasticFractal::setScaleFactor(double newScaleFactor)
{
    scaleFctr = newScaleFactor;

    emit(scaleFactorChanged(scaleFctr));
}

int StochasticFractal::countIter() const
{
    return countIterFrame;
}
void StochasticFractal::setCountIter(int newCountIter)
{
    countIterFrame = newCountIter;
    emit(countIterChanged(countIterFrame));
}

const StochasticFractal::transformations& StochasticFractal::transformation(const QString& fractal)
{
    return m[fractal];
}

template <typename T>
constexpr vec2<T> StochasticFractal::f(T x, T y, const coefficients& c) const
{
    switch (c.type)
    {
    case 0:
        return vec2<T>(c.a * x + c.b * y + c.e, c.c * x + c.d * y + c.f);

    case 1:
        return vec2<T>(c.a * std::cos(c.c) * x - c.b * std::sin(c.d) * y + c.e, c.a * std::sin(c.c) * x + c.b * std::cos(c.d) * y + c.f);

    default:
        break;
    }
}
