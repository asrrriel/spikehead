#pragma once

#include "vector.h"

template <typename T, int N, int M>
class Matrix {
    Vector<T, N> rows[M];
public:
    Matrix(Vector<T, N> v[M]) {
        for (int i = 0; i < M; i++) {
            rows[i] = v[i];
        }
    }

    Matrix(T v){
        for (int i = 0; i < M; i++) {
            rows[i] = Vector<T, N>(v);
        }
    }

    Matrix() : Matrix(T(0)) {}
};

using Mat4 = Matrix<float, 4, 4>;
using iMat4 = Matrix<int, 4, 4>;
using Mat3 = Matrix<float, 3, 3>;
using iMat3 = Matrix<int, 3, 3>;