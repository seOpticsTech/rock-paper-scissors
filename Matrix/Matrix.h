//
// Created by user on 14/02/2026.
//

#ifndef GAME_MATRIX_H
#define GAME_MATRIX_H

#include <vector>
#include "Error.h"

using namespace std;

class Matrix {
    public:
    Matrix();
    Matrix(int rows, int cols, double value = 0.0);
    double* operator[](int row);
    const double* operator[](int row) const;
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    bool operator==(const Matrix& other) const;
    static Matrix identity(int size);
    Matrix transpose() const;

    int rows;
    int cols;
    vector<double> data;
    Error err;
};

#endif //GAME_MATRIX_H
