#pragma once

#include "vec3.cpp"

#include <QVector4D>

template<typename T> struct vec4 : public vec3<T>
{
    explicit vec4(T x = {}, T y = {}, T z = {}, T w = {}) noexcept: vec3<T>(x, y, z), w(w) { }
    explicit vec4(const vec3<T>& v3, T w = {}) noexcept: vec3<T>(v3), w(w) {}
    vec4(const vec4<T>&  other) noexcept: vec3<T>(other.x, other.y, other.z), w(other.w) { }
    vec4(const vec4<T>&& other) noexcept: vec3<T>(other.x, other.y, other.z), w(other.w) { }

    explicit vec4(const QVector4D&  v) noexcept: vec3<T>(v.x(), v.y(), v.z()), w(v.w()) { }
    explicit vec4(const QVector4D&& v) noexcept: vec3<T>(v.x(), v.y(), v.z()), w(v.w()) { }

    explicit operator QVector4D() const noexcept
    {
        return QVector4D(this->x, this->y, this->z, this->w);
    }
    explicit operator std::string() const noexcept
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<T>::digits10) << static_cast<int>(this->x) << ';' << this->y << ';' << this->z << ';'<< this->w;

        return ss.str();
    }
    explicit operator QString() const noexcept
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<T>::digits10) << this->x << ';' << this->y << ';' << this->z << ';'<< this->w;

        return QString(ss.str().data());
    }

    vec4<T>& operator = (const vec4<T>&  other) noexcept
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        return *this;
    }
    vec4<T>& operator = (const vec4<T>&& other) noexcept
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        return *this;
    }

    bool operator == (const vec4& other) const noexcept
    {
        return this->x == other.x && this->y == other.y && this->z == other.z && this->w == other.w;
    }
    bool operator != (const vec4& other) const noexcept
    {
        return this->x != other.x || this->y != other.y || this->z != other.z || this->w != other.w;
    }

    T w;
};
