#include <sstream>
#include <iomanip>
#include "matrix4x4.h"

Matrix4x4::Matrix4x4() {
    for (size_t i = 0; i < 16; ++i) {
        data[i] = 0;
    }
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const { 
    Matrix4x4 result;

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            double sum = 0;
            for(size_t k = 0; k < 4; ++k) {
                sum += get(i, k) * other.get(k, j);
            }
            result.set(i, j, sum);
        }
    }

    return result; 
}

Vector4 Matrix4x4::operator*(const Vector4 &other) const {
    Vector4 result;

    for (size_t i = 0; i < 4; ++i) {
        result[i] = 0;
        for (size_t j = 0; j < 4; ++j) {
            result[i] += get(i, j) * other.c(j);
        }
    }

    return result;
}

double Matrix4x4::get(size_t row, size_t column) const {
    return data[row * 4 + column];
}

void Matrix4x4::set(size_t row, size_t column, double value) {
    data[row * 4 + column] = value;
}

std::string Matrix4x4::toString() const {
    std::ostringstream stream;

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            stream << std::setw(12) << get(i, j) << " ";
        }
        stream << std::endl;
    }    

    return stream.str();
}
