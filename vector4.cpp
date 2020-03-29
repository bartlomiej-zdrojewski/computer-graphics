#include <sstream>
#include <iomanip>
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

std::string Vector4::toString() {
    std::ostringstream stream;

    for (size_t i = 0; i < 4; ++i) {
        stream << std::setw(12) << data[i] << " ";
    }

    stream << std::endl;

    return stream.str();
}
