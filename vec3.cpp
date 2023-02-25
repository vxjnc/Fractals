#pragma once

#include "vec2.cpp"

#include <QVector3D>
#include <QColor>
#include <QRgb>

template<typename T>
struct vec3 : public vec2<T>
{
    explicit vec3(T x = {}, T y = {}, T z = {}) noexcept: vec2<T>(x, y), z(z) { }
    vec3(const vec3<T>&  other) noexcept: vec2<T>(other.x, other.y), z(other.z) { }
    vec3(const vec3<T>&& other) noexcept: vec2<T>(other.x, other.y), z(other.z) { }

    explicit vec3(const QVector3D&  v)  noexcept: vec2<T>(v.x(),       v.y()),         z(v.z()) { }
    explicit vec3(const QVector3D&& v)  noexcept: vec2<T>(v.x(),       v.y()),         z(v.z()) { }
    explicit vec3(const QColor&  other) noexcept: vec2<T>(other.red(), other.green()), z(other.blue()) { }
    explicit vec3(const QColor&& other) noexcept: vec2<T>(other.red(), other.green()), z(other.blue()) { }

    explicit operator QVector3D() const noexcept
    {
        return QVector3D(this->x, this->y, this->z);
    }
    explicit operator QRgb() const noexcept
    {
        return qRgb(this->x, this->y, this->z);
    }
    explicit operator QColor() const noexcept
    {
        return QColor(this->x, this->y, this->z);
    }
    explicit operator std::string() const noexcept
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<T>::digits10) << static_cast<int>(this->x) << ';' << static_cast<int>(this->y) << ';' << static_cast<int>(this->z);

        return ss.str();
    }
    explicit operator QString() const noexcept
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<T>::digits10) << this->x << ';' << this->y << ';' << this->z;

        return QString(ss.str().data());
    }

    vec3<T>& operator = (const vec3<T>&  other) noexcept
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        return *this;
    }
    vec3<T>& operator = (const vec3<T>&& other) noexcept
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        return *this;
    }

    bool operator == (const vec3& other) const noexcept
    {
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }
    bool operator != (const vec3& other) const noexcept
    {
        return this->x != other.x || this->y != other.y || this->z != other.z;
    }

    T z;
};
