#pragma once

#include <QVector2D>
#include <QPoint>
#include <QString>

#include <complex>
#include <iomanip>
#include <sstream>
#include <string>

template<typename T>
struct vec2
{
    explicit vec2(const T x = T{}, const T y = T{}) noexcept  : x(x), y(y) { }
    vec2(const vec2<T>& other) noexcept  : x(other.x), y(other.y) { }
    vec2(const vec2<T>&& other) noexcept : x(other.x), y(other.y) { }

    explicit vec2(const QVector2D&  v) noexcept       : x(v.x()),    y(v.y()) { }
    explicit vec2(const QVector2D&& v) noexcept       : x(v.x()),    y(v.y()) { }
    explicit vec2(const QPoint&  p) noexcept          : x(p.x()),    y(p.y()) { }
    explicit vec2(const QPoint&& p) noexcept          : x(p.x()),    y(p.y()) { }
    explicit vec2(const std::complex<T>&  c) noexcept : x(c.real()), y(c.imag()) { }
    explicit vec2(const std::complex<T>&& c) noexcept : x(c.real()), y(c.imag()) { }

    explicit operator QVector2D() const noexcept
    {
        return QVector2D(x, y);
    }
    explicit operator QPoint() const noexcept
    {
        return QPoint(x, y);
    }
    explicit operator std::string() const noexcept
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<T>::digits10) << static_cast<int>(this->x) << ';' << static_cast<int>(this->y);

        return ss.str();
    }
    explicit operator QString() const noexcept
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<T>::digits10) << x << ';' << y;

        return QString(ss.str().data());
    }
    explicit operator std::complex<T>() const noexcept
    {
        return std::complex<T>(x, y);
    }

    vec2<T>& operator = (const vec2<T>& other) noexcept
    {
        x = other.x;
        y = other.y;
        return *this;
    }
    vec2<T>& operator = (const vec2<T>&& other) noexcept
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    vec2<T> operator + (const vec2& other) const noexcept
    {
        return vec2(x + other.x, y + other.y);
    }
    vec2<T> operator + (const T other) const noexcept
    {
        return vec2(x + other, y + other);
    }

    vec2<T> operator - (const vec2& other) const noexcept
    {
        return vec2(x - other.x, y - other.y);
    }
    vec2<T> operator - (const T other) const noexcept
    {
        return vec2(x - other, y - other);
    }

    vec2<T> operator * (const vec2& other) const noexcept
    {
        return vec2(x * other.x, y * other.y);
    }
    vec2<T> operator * (const T other) const noexcept
    {
        return vec2(x * other, y * other);
    }

    vec2<T> operator / (const vec2& other) const noexcept
    {
        return vec2(x / other.x, y / other.y);
    }
    vec2<T> operator / (const T other) const noexcept
    {
        return vec2(x / other, y / other);
    }

    void operator += (const vec2& other) noexcept
    {
        x += other.x;
        y += other.y;
    }
    void operator += (const T other) noexcept
    {
        x += other;
        y += other;
    }

    void operator -= (const vec2& other) noexcept
    {
        x -= other.x;
        y -= other.y;
    }
    void operator -= (const T other) noexcept
    {
        x -= other;
        y -= other;
    }

    void operator *= (const vec2& other) noexcept
    {
        x *= other.x;
        y *= other.y;
    }
    void operator *= (T other) noexcept
    {
        x *= other;
        y *= other;
    }

    void operator /= (const vec2& other) noexcept
    {
        x /= other.x;
        y /= other.y;
    }
    void operator /= (T other) noexcept
    {
        x /= other;
        y /= other;
    }

    bool operator == (const vec2& other) const noexcept
    {
        return x == other.x && y == other.y;
    }
    bool operator != (const vec2& other) const noexcept
    {
        return x != other.x || y != other.y;
    }

    T x;
    T y;
};
