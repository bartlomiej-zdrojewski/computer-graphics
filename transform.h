#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix4x4.h"

#define PI 3.14159265

Matrix4x4 getIdentityMatrix();
Matrix4x4 getTranslationMatrix(double x, double y, double z);
Matrix4x4 getRotationXMatrix(double angle);
Matrix4x4 getRotationYMatrix(double angle);
Matrix4x4 getRotationZMatrix(double angle);
Matrix4x4 getRotationMatrix(double angle_x, double angle_y, double angle_z);
Matrix4x4 getPerspectiveMatrix(double fovy, double zNear, double zFar, double aspectRatio = 1);

#endif
