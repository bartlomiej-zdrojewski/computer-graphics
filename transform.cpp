#include <cmath>
#include "transform.h"

Matrix4x4 getIdentityMatrix() {
    Matrix4x4 matrix;

    matrix.set(0, 0, 1);
    matrix.set(1, 1, 1);
    matrix.set(2, 2, 1);
    matrix.set(3, 3, 1);

    return matrix;
}

Matrix4x4 getTranslationMatrix(double x, double y, double z) {
    Matrix4x4 matrix = getIdentityMatrix();

    matrix.set(0, 3, x);
    matrix.set(1, 3, y);
    matrix.set(2, 3, z);

    return matrix;
}

Matrix4x4 getRotationXMatrix(double angle) {
    Matrix4x4 matrix = getIdentityMatrix();

    matrix.set(1, 1, cos(angle));
    matrix.set(1, 2, -sin(angle));
    matrix.set(2, 2, cos(angle));
    matrix.set(2, 1, sin(angle));

    return matrix;
}

Matrix4x4 getRotationYMatrix(double angle) {
    Matrix4x4 matrix = getIdentityMatrix();

    matrix.set(0, 0, cos(angle));
    matrix.set(0, 2, sin(angle));
    matrix.set(2, 0, -sin(angle));
    matrix.set(2, 2, cos(angle));

    return matrix;
}

Matrix4x4 getRotationZMatrix(double angle) {
    Matrix4x4 matrix = getIdentityMatrix();

    matrix.set(0, 0, cos(angle));
    matrix.set(0, 1, -sin(angle));
    matrix.set(1, 0, sin(angle));
    matrix.set(1, 1, cos(angle));

    return matrix;
}

Matrix4x4 getRotationMatrix(double angle_x, double angle_y, double angle_z) {
    return getRotationZMatrix(angle_z) * getRotationYMatrix(angle_y) * getRotationXMatrix(angle_x);
}

Matrix4x4 getPerspectiveMatrix(double fovy, double zNear, double zFar, double aspectRatio) {
    Matrix4x4 matrix;

    matrix.set(0, 0, 1 / tan(fovy / 2) / aspectRatio);
    matrix.set(1, 1, 1 / tan(fovy / 2));
    matrix.set(2, 2, (zFar + zNear) / (zFar - zNear));
    matrix.set(2, 3, (2 * zFar * zNear) / (zNear - zFar));
    matrix.set(3, 2, -1);

    return matrix;
}
