//
// Created by user on 14/02/2026.
//

#ifndef GAME_VECTOR_H
#define GAME_VECTOR_H

#include "Matrix/Matrix.h"
#include "Error.h"

using namespace std;

class Vector {
    public:
    Vector();
    Vector(double x, double y);
    explicit Vector(const Matrix& matrix);

    double& operator[](int index);
    const double& operator[](int index) const;
    Vector operator+(const Vector& other) const;
    Vector operator+(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    bool operator==(const Vector& other) const;
    Matrix transpose() const;
    static Matrix identity();
    Matrix toMatrix() const;

    Error err;

    protected:
    Matrix matrix;
};

Matrix operator+(const Matrix& left, const Vector& right);
Vector operator*(const Matrix& left, const Vector& right);

#endif //GAME_VECTOR_H
