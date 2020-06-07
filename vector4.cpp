#include <cmath>
#include <iomanip>
#include <sstream>
#include "vector4.h"

Vector4::Vector4(double x, double y, double z, double w) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

double& Vector4::operator[](size_t const &i) {
    return data[i];
}

double Vector4::c(size_t const &i) const {
    return data[i];
}

double& Vector4::x() {
    return data[0];
}

double& Vector4::y() {
    return data[1];
}

double& Vector4::z() {
    return data[2];
}

double& Vector4::w() {
    return data[3];
}

double Vector4::cx() const {
    return data[0];
}

double Vector4::cy() const {
    return data[1];
}

double Vector4::cz() const {
    return data[2];
}

double Vector4::cw() const {
    return data[3];
}

double Vector4::getLength() const {
    double sumOfSquares = 0;

    for (size_t i = 0; i < 4; ++i) {
        sumOfSquares += data[i] * data[i];
    }

    return sqrt(sumOfSquares);
}

void Vector4::normalize() {
    double length = getLength();

    if (length == 0.0) {
        return;
    }
    
    for (size_t i = 0; i < 4; ++i) {
        data[i] /= length;
    }
}

std::string Vector4::toString() const {
    std::ostringstream stream;

    for (size_t i = 0; i < 4; ++i) {
        stream << std::setw(12) << data[i] << " ";
    }

    stream << std::endl;

    return stream.str();
}
