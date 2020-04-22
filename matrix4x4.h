#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include <string>
#include "vector4.h"

class Matrix4x4 {

public:

    Matrix4x4();

    Matrix4x4 operator*(const Matrix4x4 &other) const;
    Vector4 operator*(const Vector4 &other) const;

    double get(size_t row, size_t column) const;
    void set(size_t row, size_t column, double value);

    std::string toString() const;

private:

    double data[16];
    
};

#endif
