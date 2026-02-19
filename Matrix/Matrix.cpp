//
// Created by user on 14/02/2026.
//

#include "Matrix.h"

Matrix::Matrix() : rows(0), cols(0), data(), err() {}

Matrix::Matrix(int rows, int cols, double value)
    : rows(rows), cols(cols), data(static_cast<size_t>(rows * cols), value), err() {}

double* Matrix::operator[](int row) {
    return data.data() + static_cast<size_t>(row * cols);
}

const double* Matrix::operator[](int row) const {
    return data.data() + static_cast<size_t>(row * cols);
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        Matrix result;
        result.err = Error::New("Matrix add dimension mismatch");
        return result;
    }

    Matrix result(rows, cols, 0.0);
    const int count = rows * cols;
    for (int i = 0; i < count; ++i) {
        result.data[static_cast<size_t>(i)] = data[static_cast<size_t>(i)] + other.data[static_cast<size_t>(i)];
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        Matrix result;
        result.err = Error::New("Matrix add dimension mismatch");
        return result;
    }

    Matrix result(rows, cols, 0.0);
    const int count = rows * cols;
    for (int i = 0; i < count; ++i) {
        result.data[static_cast<size_t>(i)] = data[static_cast<size_t>(i)] - other.data[static_cast<size_t>(i)];
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        Matrix result;
        result.err = Error::New("Matrix multiply dimension mismatch");
        return result;
    }

    Matrix result(rows, other.cols, 0.0);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < other.cols; ++c) {
            double sum = 0.0;
            for (int k = 0; k < cols; ++k) {
                sum += (*this)[r][k] * other[k][c];
            }
            result[r][c] = sum;
        }
    }
    return result;
}

bool Matrix::operator==(const Matrix& other) const {
    return rows == other.rows && cols == other.cols && data == other.data;
}

Matrix Matrix::identity(int size) {
    Matrix result(size, size, 0.0);
    for (int i = 0; i < size; ++i) {
        result[i][i] = 1.0;
    }
    return result;
}

Matrix Matrix::transpose() const {
    Matrix result(cols, rows, 0.0);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            result[c][r] = (*this)[r][c];
        }
    }
    return result;
}
