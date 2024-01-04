//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_VECTORS_H
#define LIVEWALLPAPER05_VECTORS_H


#include <string>
#include "cyCodeBase-master/cyMatrix.h"

using std::string;
using cy::Matrix3;
using cy::Matrix4;
using cy::Vec3;

template <class T>
class _vec2 {
public:
    union {
        T v[2];
        struct {
            T x;
            T y;
        };
        struct {
            T r;
            T g;
        };
    };
    T& operator[](const int& index) {
        return v[index];
    }
    _vec2() {

    }
    _vec2(const T& input) {
        for (int i = 0; i < 2; i++) v[i] = input;
    }
    _vec2(const T& x, const T& y) {
        this->x = x; this->y = y;
    }
    _vec2(const T array[2]) {
        for (int i = 0; i < 2; i++) v[i] = array[i];
    }
    _vec2(const _vec2<unsigned int>& input) {
        for (int i = 0; i < 2; i++) v[i] = (T)input.v[i];
    }
    _vec2(const _vec2<int>& input) {
        for (int i = 0; i < 2; i++) v[i] = (T)input.v[i];
    }
    _vec2(const _vec2<float>& input) {
        for (int i = 0; i < 2; i++) v[i] = (T)input.v[i];
    }
    _vec2(const _vec2<double>& input) {
        for (int i = 0; i < 2; i++) v[i] = (T)input.v[i];
    }
    _vec2 operator+(const _vec2& input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] + input[i];
        return _return;
    }
    _vec2 operator+(const _vec2& input) const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] + input[i];
        return _return;
    }
    _vec2& operator+=(const _vec2& input) {
        for (int i = 0; i < 2; i++) v[i] += input[i];
        return *this;
    }
    _vec2 operator-(const _vec2& input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] - input[i];
        return _return;
    }
    _vec2 operator-(const _vec2& input) const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] - input[i];
        return _return;
    }
    _vec2& operator-=(const _vec2& input) {
        for (int i = 0; i < 2; i++) v[i] -= input[i];
        return *this;
    }
    _vec2 operator-() {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = -v[i];
        return _return;
    }
    _vec2 operator-() const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = -v[i];
        return _return;
    }
    _vec2 operator*(const _vec2& input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] * input.v[i];
        return _return;
    }
    _vec2 operator*(const _vec2& input) const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] * input.v[i];
        return _return;
    }
    _vec2& operator*=(const _vec2& input) {
        for (int i = 0; i < 2; i++) v[i] *= input[i];
        return *this;
    }
    _vec2 operator*(const T& scalar) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec2 operator*(const T& scalar) const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec2& operator*=(const T& scalar) {
        for (int i = 0; i < 2; i++) v[i] *= scalar;
        return *this;
    }
    _vec2 operator/(const _vec2& input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] / input[i];
        return _return;
    }
    _vec2 operator/(const _vec2& input) const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] / input[i];
        return _return;
    }
    _vec2& operator/=(const _vec2& input) {
        for (int i = 0; i < 2; i++) v[i] /= input[i];
        return *this;
    }
    _vec2 operator/(const T& input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec2 operator/(const T& input) const {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec2& operator/=(const T& input) {
        for (int i = 0; i < 2; i++) v[i] /= input;
        return *this;
    }
    _vec2& operator=(const _vec2<unsigned int>& rhs) {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(const _vec2<unsigned int>& rhs) const {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(const _vec2<int>& rhs) {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(const _vec2<int>& rhs) const {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(const _vec2<float>& rhs) {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(const _vec2<float>& rhs) const {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    bool operator==(const _vec2& rhs) {
        return rhs.x == x && rhs.y == y;
    }
    bool operator==(const _vec2& rhs) const {
        return rhs.x == x && rhs.y == y;
    }
    bool operator!=(const _vec2& rhs) {
        return rhs.x != x || rhs.y != y;
    }
    bool operator!=(const _vec2& rhs) const {
        return rhs.x != x || rhs.y != y;
    }
    string str() {
        return "(" + to_string(x) + ", " + to_string(y) + ")";
    }
    string str() const {
        return "(" + to_string(x) + ", " + to_string(y) + ")";
    }
};

template <typename T>
string to_string(const _vec2<T>& input) {
    return input.str();
}

template <typename T>
_vec2<T> operator*(const T& lhs, const _vec2<T>& rhs) {
    _vec2<T> _return;
    for (int i = 0; i < 2; i++) _return[i] = lhs * (T)rhs.v[i];
    return _return;
}

_vec2<float> operator*(const float& lhs, const _vec2<int>& rhs) {
    _vec2<float> _return;
    for (int i = 0; i < 2; i++) _return[i] = lhs * (float)rhs.v[i];
    return _return;
}
_vec2<float> operator*(const _vec2<int>& lhs, const float& rhs) {
    return rhs * lhs;
}
_vec2<float> operator*(const int& lhs, const _vec2<float>& rhs) {
    _vec2<float> _return;
    for (int i = 0; i < 2; i++) _return[i] = (float)lhs * rhs.v[i];
    return _return;
}
_vec2<float> operator*(const _vec2<float>& lhs, const int& rhs) {
    return rhs * lhs;
}

_vec2<double> operator*(const double& lhs, const _vec2<float>& rhs) {
    _vec2<double> _return;
    for (int i = 0; i < 2; i++) _return[i] = lhs * (double)rhs.v[i];
    return _return;
}
_vec2<double> operator*(const _vec2<float>& lhs, const double& rhs) {
    return rhs * lhs;
}
_vec2<double> operator*(const float& lhs, const _vec2<double>& rhs) {
    _vec2<double> _return;
    for (int i = 0; i < 2; i++) _return[i] = (double)lhs * rhs.v[i];
    return _return;
}
_vec2<double> operator*(const _vec2<double>& lhs, const float& rhs) {
    return rhs * lhs;
}

template <class T>
class _vec3 {
public:
    union {
        T v[3];
        _vec2<T> xy;
        struct {
            T x;
            T y;
            T z;
        };
        struct {
            T r;
            T g;
            T b;
        };
    };
    T& operator[](const int& index) {
        return v[index];
    }
    _vec3() {

    }
    _vec3(const T& input) {
        for (int i = 0; i < 3; i++) v[i] = input;
    }
    _vec3(const T& x, const T& y, const T& z) {
        this->x = x; this->y = y; this->z = z;
    }
    _vec3(const T array[3]) {
        for (int i = 0; i < 3; i++) v[i] = array[i];
    }
    _vec3(const _vec2<T>& input, const T& z) {//Should try reducing to just one loop of 3 iterations
        for (int i = 0; i < 2; i++) v[i] = input[i]; v[2] = z;
    }
    _vec3(const _vec3<unsigned int>& input) {
        for (int i = 0; i < 3; i++) v[i] = (T)input.v[i];
    }
    _vec3(const _vec3<int>& input) {
        for (int i = 0; i < 3; i++) v[i] = (T)input.v[i];
    }
    _vec3(const _vec3<float>& input) {
        for (int i = 0; i < 3; i++) v[i] = (T)input.v[i];
    }
    _vec3(const _vec3<double>& input) {
        for (int i = 0; i < 3; i++) v[i] = (T)input.v[i];
    }
    _vec3 operator+(const _vec3& input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] + input.v[i];
        return _return;
    }
    _vec3 operator+(const _vec3& input) const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] + input.v[i];
        return _return;
    }
    _vec3& operator+=(const _vec3& input) {
        for (int i = 0; i < 3; i++) v[i] += input.v[i];
        return *this;
    }
    _vec3 operator-(const _vec3& input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] - input.v[i];
        return _return;
    }
    _vec3 operator-(const _vec3& input) const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] - input.v[i];
        return _return;
    }
    _vec3& operator-=(const _vec3& input) {
        for (int i = 0; i < 3; i++) v[i] -= input.v[i];
        return *this;
    }
    _vec3 operator-() const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = -v[i];
        return _return;
    }
    _vec3 operator-() {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = -v[i];
        return _return;
    }
    _vec3 operator*(const _vec3& input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] * input.v[i];
        return _return;
    }
    _vec3 operator*(const _vec3& input) const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] * input.v[i];
        return _return;
    }
    _vec3& operator*=(const _vec3& input) {
        for (int i = 0; i < 3; i++) v[i] *= input.v[i];
        return *this;
    }
    _vec3 operator*(const T& scalar) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec3 operator*(const T& scalar) const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec3& operator*=(const T& scalar) {
        for (int i = 0; i < 3; i++) v[i] *= scalar;
        return *this;
    }
    _vec3 operator/(const _vec3& input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] / input.v[i];
        return _return;
    }
    _vec3 operator/(const _vec3& input) const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] / input.v[i];
        return _return;
    }
    _vec3& operator/=(const _vec3& input) {
        for (int i = 0; i < 3; i++) v[i] /= input.v[i];
        return *this;
    }
    _vec3 operator/(const T& input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec3 operator/(const T& input) const {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec3& operator/=(const T& input) {
        for (int i = 0; i < 3; i++) v[i] /= input;
        return *this;
    }
    _vec3& operator=(const _vec3<unsigned int>& rhs) {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(const _vec3<unsigned int>& rhs) const {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(const _vec3<int>& rhs) {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(const _vec3<int>& rhs) const {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(const _vec3<float>& rhs) {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(const _vec3<float>& rhs) const {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    bool operator==(const _vec3& rhs) {
        return rhs.x == x && rhs.y == y && rhs.z == z;
    }
    bool operator==(const _vec3& rhs) const {
        return rhs.x == x && rhs.y == y && rhs.z == z;
    }
    bool operator!=(const _vec3& rhs) {
        return rhs.x != x || rhs.y != y || rhs.z != z;
    }
    bool operator!=(const _vec3& rhs) const {
        return rhs.x != x || rhs.y != y || rhs.z != z;
    }
    string str() {
        return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
    }
    string str() const {
        return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
    }
};

template <typename T>
string to_string(const _vec3<T>& input) {
    return input.str();
}

template <typename T>
_vec3<T> operator*(const T& lhs, const _vec3<T>& rhs) {
    _vec3<T> _return;
    for (int i = 0; i < 3; i++) _return[i] = lhs * (T)rhs.v[i];
    return _return;
}

_vec3<float> operator*(const float& lhs, const _vec3<int>& rhs) {
    _vec3<float> _return;
    for (int i = 0; i < 3; i++) _return[i] = lhs * (float)rhs.v[i];
    return _return;
}
_vec3<float> operator*(const _vec3<int>& lhs, const float& rhs) {
    return rhs * lhs;
}
_vec3<float> operator*(const int& lhs, const _vec3<float>& rhs) {
    _vec3<float> _return;
    for (int i = 0; i < 3; i++) _return[i] = (float)lhs * rhs.v[i];
    return _return;
}
_vec3<float> operator*(const _vec3<float>& lhs, const int& rhs) {
    return rhs * lhs;
}

_vec3<double> operator*(const double& lhs, const _vec3<float>& rhs) {
    _vec3<double> _return;
    for (int i = 0; i < 3; i++) _return[i] = lhs * (double)rhs.v[i];
    return _return;
}
_vec3<double> operator*(const _vec3<float>& lhs, const double& rhs) {
    return rhs * lhs;
}
_vec3<double> operator*(const float& lhs, const _vec3<double>& rhs) {
    _vec3<double> _return;
    for (int i = 0; i < 3; i++) _return[i] = (double)lhs * rhs.v[i];
    return _return;
}
_vec3<double> operator*(const _vec3<double>& lhs, const float& rhs) {
    return rhs * lhs;
}

template <class T>
class _vec4 {
public:
    union {
        T v[4];
        _vec2<T> xy;
        _vec3<T> xyz;
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        struct {
            T r;
            T g;
            T b;
            T a;
        };
    };
    T& operator[](const int& index) {
        return v[index];
    }
    _vec4() {

    }
    _vec4(const T& input) {
        for (int i = 0; i < 4; i++) v[i] = input;
    }
    _vec4(const T& x, const T& y, const T& z, const T& w) {
        this->x = x; this->y = y; this->z = z; this->w = w;
    }
    _vec4(const T array[4]) {
        for (int i = 0; i < 4; i++) v[i] = array[i];
    }
    _vec4(const _vec3<T>& input, const T& z) {//Should try reducing to just one loop of 4 iterations
        for (int i = 0; i < 3; i++) v[i] = (T)input.v[i]; v[3] = (T)z;
    }
    _vec4(const _vec4<unsigned int>& input) {
        for (int i = 0; i < 4; i++) v[i] = (T)input.v[i];
    }
    _vec4(const _vec4<int>& input) {
        for (int i = 0; i < 4; i++) v[i] = (T)input.v[i];
    }
    _vec4(const _vec4<float>& input) {
        for (int i = 0; i < 4; i++) v[i] = (T)input.v[i];
    }
    _vec4(const _vec4<double>& input) {
        for (int i = 0; i < 4; i++) v[i] = (T)input.v[i];
    }
    _vec4 operator+(const _vec4& input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] + input.v[i];
        return _return;
    }
    _vec4 operator+(const _vec4& input) const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] + input.v[i];
        return _return;
    }
    _vec4& operator+=(const _vec4& input) {
        for (int i = 0; i < 4; i++) v[i] += input.v[i];
        return *this;
    }
    _vec4 operator-(const _vec4& input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] - input.v[i];
        return _return;
    }
    _vec4 operator-(const _vec4& input) const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] - input.v[i];
        return _return;
    }
    _vec4& operator-=(const _vec4& input) {
        for (int i = 0; i < 4; i++) v[i] -= input.v[i];
        return *this;
    }
    _vec4 operator-() {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = -v[i];
        return _return;
    }
    _vec4 operator-() const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = -v[i];
        return _return;
    }
    _vec4 operator*(const _vec4& input) const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] * input.v[i];
        return _return;
    }
    _vec4 operator*(const _vec4& input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] * input.v[i];
        return _return;
    }
    _vec4& operator*=(const _vec4& input) {
        for (int i = 0; i < 4; i++) v[i] *= input.v[i];
        return *this;
    }
    _vec4 operator*(const T& scalar) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec4 operator*(const T& scalar) const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec4& operator*=(const T& scalar) {
        for (int i = 0; i < 4; i++) v[i] *= scalar;
        return *this;
    }
    _vec4 operator/(const _vec4& input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] / input.v[i];
        return _return;
    }
    _vec4 operator/(const _vec4& input) const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] / input.v[i];
        return _return;
    }
    _vec4& operator/=(const _vec4& input) {
        for (int i = 0; i < 4; i++) v[i] /= input.v[i];
        return *this;
    }
    _vec4 operator/(const T& input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec4 operator/(const T& input) const {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec4& operator/=(const T& input) {
        for (int i = 0; i < 4; i++) v[i] /= input;
        return *this;
    }
    _vec4& operator=(const _vec4<unsigned int>& rhs) {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(const _vec4<unsigned int>& rhs) const {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(const _vec4<int>& rhs) {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(const _vec4<int>& rhs) const {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(const _vec4<float>& rhs) {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(const _vec4<float>& rhs) const {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    bool operator==(const _vec4& rhs) {
        return rhs.x == x && rhs.y == y && rhs.z == z && rhs.w == w;
    }
    bool operator==(const _vec4& rhs) const {
        return rhs.x == x && rhs.y == y && rhs.z == z && rhs.w == w;
    }
    bool operator!=(const _vec4& rhs) {
        return rhs.x != x || rhs.y != y || rhs.z != z || rhs.w != w;
    }
    bool operator!=(const _vec4& rhs) const {
        return rhs.x != x || rhs.y != y || rhs.z != z || rhs.w != w;
    }
    string str() {
        return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ", " + to_string(w) + ")";
    }
    string str() const {
        return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ", " + to_string(w) + ")";
    }
};

template <typename T>
string to_string(const _vec4<T>& input) {
    return input.str();
}

template <typename T>
_vec4<T> operator*(const T& lhs, const _vec4<T>& rhs) {
    _vec4<T> _return;
    for (int i = 0; i < 4; i++) _return[i] = lhs * (T)rhs.v[i];
    return _return;
}

_vec4<float> operator*(const float& lhs, const _vec4<int>& rhs) {
    _vec4<float> _return;
    for (int i = 0; i < 4; i++) _return[i] = lhs * (float)rhs.v[i];
    return _return;
}
_vec4<float> operator*(const _vec4<int>& lhs, const float& rhs) {
    return rhs * lhs;
}
_vec4<float> operator*(const int& lhs, const _vec4<float>& rhs) {
    _vec4<float> _return;
    for (int i = 0; i < 4; i++) _return[i] = (float)lhs * rhs.v[i];
    return _return;
}
_vec4<float> operator*(const _vec4<float>& lhs, const int& rhs) {
    return rhs * lhs;
}

_vec4<double> operator*(const double& lhs, const _vec4<float>& rhs) {
    _vec4<double> _return;
    for (int i = 0; i < 4; i++) _return[i] = lhs * (double)rhs.v[i];
    return _return;
}
_vec4<double> operator*(const _vec4<float>& lhs, const double& rhs) {
    return rhs * lhs;
}
_vec4<double> operator*(const float& lhs, const _vec4<double>& rhs) {
    _vec4<double> _return;
    for (int i = 0; i < 4; i++) _return[i] = (double)lhs * rhs.v[i];
    return _return;
}
_vec4<double> operator*(const _vec4<double>& lhs, const float& rhs) {
    return rhs * lhs;
}

typedef _vec2<float> vec2;
typedef _vec2<unsigned int> uvec2;
typedef _vec2<int> ivec2;
typedef _vec2<unsigned char> ucvec2;

typedef _vec3<float> vec3;
typedef _vec3<unsigned int> uvec3;
typedef _vec3<int> ivec3;
typedef _vec3<unsigned char> ucvec3;
const vec3 a = vec3(1.0f);
const vec3 b = vec3(1.0f);
vec3 sample3 = a + b;
typedef _vec4<float> vec4;
typedef _vec4<unsigned int> uvec4;
typedef _vec4<int> ivec4;
typedef _vec4<unsigned char> ucvec4;

template<class T>
_vec3<T> cross(const _vec3<T>& left, const _vec3<T>& right) {
    return _vec3<T>(
            left.y * right.z - left.z * right.y,
            -left.x * right.z + left.z * right.x,
            left.x * right.y - left.y * right.x
    );
}

template<class T>
T dot(const _vec2<T>& left, const _vec2<T>& right) {
    _vec2<T> product = left * right;
    return product.x + product.y;
}

template<class T>
T dot(const _vec3<T>& left, const _vec3<T>& right) {
    _vec3<T> product = left * right;
    return product.x + product.y + product.z;
}

template<class T>
T dot(const _vec4<T>& left, const _vec4<T>& right) {
    _vec4<T> product = left * right;
    return product.x + product.y + product.z + product.w;
}

template<class T>
T distance(const _vec2<T>& coordinates) {
    return sqrt(dot(coordinates, coordinates));
}

template<class T>
T distance(const _vec3<T>& coordinates) {
    return sqrt(dot(coordinates, coordinates));
}

template<class T>
T distance(const _vec2<T>& coordinates1, const _vec2<T>& coordinates2) {
    return distance(coordinates2 - coordinates1);
}

template<class T>
T distance(const _vec3<T>& coordinates1, const _vec3<T>& coordinates2) {
    return distance(coordinates2 - coordinates1);
}

template<class T>
T length(const _vec2<T>& coordinates) {
    return distance(coordinates);
}

template<class T>
T length(const _vec3<T>& coordinates) {
    return distance(coordinates);
}

template <class T>
void clamp(const int& vectorDimensionCount, T* const x, T* const minVal, T* const maxVal) {
    for (int i = 0; i < vectorDimensionCount; i++) {
        if (x[i] < minVal[i])
            x[i] = minVal[i];
        if (x[i] > maxVal[i])
            x[i] = maxVal[i];
    }
}

template<class T>
T clamp(const T& x, const T& minVal, const T& maxVal) {
    clamp<T>(1, (T*)&x, (T*)&minVal, (T*)&maxVal);
    return x;
}

template<class T>
_vec2<T> clamp(const _vec2<T>& x, const _vec2<T>& minVal, const _vec2<T>& maxVal) {
    clamp<T>(2, (T*)&x, (T*)&minVal, (T*)&maxVal);
    return x;
}

template<class T>
_vec3<T> clamp(const _vec3<T>& x, const _vec3<T>& minVal, const _vec3<T>& maxVal) {
    clamp<T>(3, (T*)&x, (T*)&minVal, (T*)&maxVal);
    return x;
}

template<class T>
_vec4<T> clamp(const _vec4<T>& x, const _vec4<T>& minVal, const _vec4<T>& maxVal) {
    clamp<T>(4, (T*)&x, (T*)&minVal, (T*)&maxVal);
    return x;
}

template<class T>
T distance(const T& x, const T& y) {
    return sqrt(x * x + y * y);
}

template<class T>
T distance(const T& x, const T& y, const T& z) {
    return sqrt(x * x + y * y + z * z);
}

template<class T>
T toDegrees(const T& radians) {
    return radians * 180.0 / M_PI;
}

template<class T>
T toRadians(const T& degrees) {
    return degrees * M_PI / 180.0;
}

template<class T>
_vec2<T> normalize(const _vec2<T>& input) {
    return input / distance(input);
}

template<class T>
_vec3<T> normalize(const _vec3<T>& input) {
    return input / distance(input);
}

template<class T>
Matrix3<T> quaternionTo3x3(const _vec4<T>& input) {
    T thetaOver2 = acosf(input.v[3]);
    T sinOfHalfTheta = sinf(thetaOver2);
    T x = input.v[0] / sinOfHalfTheta;
    T y = input.v[1] / sinOfHalfTheta;
    T z = input.v[2] / sinOfHalfTheta;
    T theta = 2.0f * thetaOver2;
    return Matrix3<T>(
            Vec3<T>(x * x * (1 - cosf(theta)) + cosf(theta), x * y * (1 - cosf(theta)) - z * sinf(theta), x * z * (1 - cosf(theta)) + y * sinf(theta)),
            Vec3<T>(x * y * (1 - cosf(theta)) + z * sinf(theta), y * y * (1 - cosf(theta)) + cosf(theta), y * z * (1 - cosf(theta)) - x * sinf(theta)),
            Vec3<T>(x * z * (1 - cosf(theta)) - y * sinf(theta), y * z * (1 - cosf(theta)) + x * sinf(theta), z * z * (1 - cosf(theta)) + cosf(theta))
    );
}


#endif //LIVEWALLPAPER05_VECTORS_H
