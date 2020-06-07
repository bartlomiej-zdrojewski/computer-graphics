#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <vector>
#include "light_engine.h"
#include "vertex.h"

class RenderEngine {

public:

    RenderEngine();
    
    LightEngine& getLightEngine();

    double getViewWidth() const;
    double getViewHeight() const;
    void setViewSize(size_t viewWidth, size_t viewHeight);

    Vector4 getBackgroundColor() const;
    void setBackgroundColor(Vector4 backgroundColor);

    void setVertexArray(const std::vector<Vertex> &vertexArray);

    void run();
    const std::vector<Vector4>& getImage();

private:

    Vector4 getGradientColor(const std::vector<Vertex> &primitive, Vector4 position);
    Vector4 getLight(const std::vector<Vertex> &primitive, Vector4 position);

private:

    LightEngine lightEngine;

    size_t viewWidth, viewHeight;
    Vector4 backgroundColor;

    std::vector<Vertex> vertexArray;
    std::vector<Vector4> image;

};

#endif
