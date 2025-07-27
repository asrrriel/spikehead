#pragma once

#include "vector.h"

template <typename T, int N, int M>
class Matrix {
    T data[M*N];
public:
    Matrix(Matrix &m) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                data[i*N + j] = m.data[i*N + j];
            }
        }
    }

    Matrix(T v[M*N]) {
        for (int i = 0; i < M*N; i++) {
            data[i] = v[i];
        }
    }

    Matrix(Vector<T, N> v[M]) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                data[i*N + j] = v[i][j];
            }
        }
    }

    Matrix(T v){
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                data[i*N + j] = v;
            }
        }
    }

    Matrix() : Matrix(T(0)) {}

    T& operator()(int i, int j) {
        return data[i*N + j];
    }

    Vector<T, N> operator[](int i) {
        Vector<T, N> result;
        for (int j = 0; j < N; j++) {
            result[j] = data[i*N + j];
        }
        return result;
    }

    Matrix operator+(Matrix m) {
        Matrix result;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                result.data[i*N + j] = data[i*N + j] + m.data[i*N + j];
            }
        }
        return result;
    }

    Matrix operator-(Matrix m) {
        Matrix result;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                result.data[i*N + j] = data[i*N + j] - m.data[i*N + j];
            }
        }
        return result;
    }

    Matrix operator*(Matrix m) {
        Matrix result;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                result.data[i * N + j] = 0;
                for(int k = 0; k < N; k++) {
                    result.data[i*N + j] += data[k*N + j] * m.data[i*N + k];
                }
            }
        }
        return result;
    }

Matrix& operator*=(const Matrix& m) {
    Matrix result;

    for (int i = 0; i < M; i++) {         // row
        for (int j = 0; j < N; j++) {     // column
            result.data[i * N + j] = 0;
            for (int k = 0; k < N; k++) {
                result.data[i * N + j] += data[i * N + k] * m.data[k * N + j];
            }
        }
    }

    *this = result;
    return *this;
}

    float* get_data() {
        return data;
    }
};

using Mat2 = Matrix<float, 2, 2>;
using iMat2 = Matrix<int, 2, 2>;

using Mat3 = Matrix<float, 3, 3>;
using iMat3 = Matrix<int, 3, 3>;

using Mat4 = Matrix<float, 4, 4>;
using iMat4 = Matrix<int, 4, 4>;