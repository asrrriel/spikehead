#pragma once

template <typename T, int N>
class Vector
{
    T data[N];

public:
    Vector(T v[N]) {
        for (int i = 0; i < N; i++) {
            data[i] = v[i];
        }
    }

    Vector(T v) {
        for (int i = 0; i < N; i++) {
            data[i] = v;
        }
    }

    Vector() : Vector(T(0)) {}


    T& operator[](int i) {
        return data[i];
    }

    Vector operator+(Vector v) {
        Vector result;
        for (int i = 0; i < N; i++) {
            result[i] = data[i] + v[i];
        }
        return result;
    }

    Vector operator-(Vector v) {
        Vector result;
        for (int i = 0; i < N; i++) {
            result[i] = data[i] - v[i];
        }
        return result;
    }

    Vector operator*(Vector v) {
        Vector result;
        for (int i = 0; i < N; i++) {
            result[i] = data[i] * v[i];
        }
        return result;
    }

    Vector operator/(Vector v) {
        Vector result;
        for (int i = 0; i < N; i++) {
            result[i] = data[i] / v[i];
        }
        return result;
    }

    T dot(Vector v) {
        T result = 0;
        for (int i = 0; i < N; i++) {
            result += data[i] * v[i];
        }
        return result;
    }
};

using Vec2f = Vector<float, 2>;
using Vec2i = Vector<int, 2>;

using Vec3f = Vector<float, 3>;
using Vec3i = Vector<int, 3>;