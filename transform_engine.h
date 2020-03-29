#ifndef TRANSFORM_ENGINE_H
#define TRANSFORM_ENGINE_H

#include <vector>
#include "clip_engine.h"
#include "transform.h"

class TransformEngine {
    
public:

    TransformEngine();

    void loadModelFromVertexArray(std::vector<Vector4> &vertexArray);
    bool loadModelFromBuffer(const std::string &buffer);
    bool loadModelFromFile(const std::string &filePath);
    void clearModel();

    double getViewWidth();
    double getViewHeight();
    void setViewSize(double viewWidth, double viewHeight);

    Vector4 getViewTranslation() const;
    void setViewTranslation(Vector4 viewTranslation);
    void setViewTranslation(double x, double y, double z);

    Vector4 getViewRotation() const;
    void setViewRotation(Vector4 viewRotation);
    void setViewRotation(double angle_x, double angle_y, double angle_z);

    double getFovy() const;
    double getZNear() const;
    double getZFar() const;
    void setPerspective(double fovy, double zNear, double zFar);

    void run();
    std::vector<Vector4> getTransformedVertexes() const;

private:

    ClipEngine clipEngine;

    double viewWidth, viewHeight;
    Vector4 viewTranslation;
    Vector4 viewRotation;
    double fovy, zNear, zFar;

    std::vector<Vector4> modelVertexes;
    std::vector<Vector4> transformedVertexes;
};

#endif
