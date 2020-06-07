#ifndef VECTOR4_H
#define VECTOR4_H

#include <string>

class Vector4 {

public:

    Vector4(double x = 0, double y = 0, double z = 0, double w = 0);

    double& operator[](size_t const &i);
    double c(size_t const &i) const;

    double& x();
    double& y();
    double& z();
    double& w();

    double cx() const;
    double cy() const;
    double cz() const;
    double cw() const;

    double getLength() const;
    void normalize();

    std::string toString() const;

private:

    double data[4];

};

#endif
