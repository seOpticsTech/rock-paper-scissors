//
// Created by user on 14/02/2026.
//

#include "Vector.h"
#include "Matrix/Matrix.h"

Vector::Vector() : matrix(2, 1, 0.0), err() {}

Vector::Vector(double x, double y) : matrix(2, 1, 0.0), err() {
    matrix[0][0] = x;
    matrix[1][0] = y;
}

Vector::Vector(const Matrix& matrix) : matrix(matrix), err() {
    if (matrix.rows != 2 || matrix.cols != 1) {
        err = Error::New("Vector must be 2x1");
        this->matrix = Matrix();
    }
}

double& Vector::operator[](int index) {
    return matrix[index][0];
}

const double& Vector::operator[](int index) const {
    return matrix[index][0];
}

Vector Vector::operator+(const Vector& other) const {
    Vector result;
    result.matrix = matrix + other.matrix;
    result.err = result.matrix.err;
    return result;
}

Vector Vector::operator+(const Matrix& other) const {
    if (other.err.status == failure) {
        Vector result;
        result.err = other.err;
        return result;
    }

    Vector result;
    result.matrix = matrix + other;
    result.err = result.matrix.err;
    return result;
}

Matrix Vector::operator*(const Matrix& other) const {
    if (other.err.status == failure) {
        Matrix result;
        result.err = other.err;
        return result;
    }
    return matrix * other;
}

bool Vector::operator==(const Vector& other) const {
    return matrix == other.matrix;
}

Matrix Vector::transpose() const {
    return matrix.transpose();
}

Matrix Vector::identity() {
    return Matrix::identity(2);
}

Matrix Vector::toMatrix() const {
    return matrix;
}

Matrix operator+(const Matrix& left, const Vector& right) {
    if (left.err.status == failure) {
        Matrix result;
        result.err = left.err;
        return result;
    }
    if (right.err.status == failure) {
        Matrix result;
        result.err = right.err;
        return result;
    }
    return left + right.toMatrix();
}

Vector operator*(const Matrix& left, const Vector& right) {
    if (left.err.status == failure) {
        Vector result;
        result.err = left.err;
        return result;
    }
    if (right.err.status == failure) {
        Vector result;
        result.err = right.err;
        return result;
    }

    Matrix result = left * right.toMatrix();
    if (result.err.status == failure) {
        Vector vec;
        vec.err = result.err;
        return vec;
    }
    return Vector(result);
}
