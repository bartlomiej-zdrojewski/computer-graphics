#ifndef VECTOR4_H
#define VECTOR4_H

#include <string>

class Vector4 {

public:

    Vector4(double x = 0, double y = 0, double z = 0, double w = 0);

    double& operator[](size_t const &i);

    double& x();
    double& y();
    double& z();
    double& w();

    std::string toString();

private:

    double data[4];

};

#endif
