#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "vector4.h"

struct LightSource {

    LightSource() : LightSource(Vector4(), 1.0, Vector4(1.0, 1.0, 1.0, 1.0)) {}

    LightSource(Vector4 position, double intensity, Vector4 color) {
        this->position = position;
        this->intensity = intensity;
        this->color = color;
    }

    Vector4 position;
    double intensity;
    Vector4 color;
};

#endif
