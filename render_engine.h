#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <vector>
#include "vertex.h"

class RenderEngine {

public:

    RenderEngine();

    void setVertexArray(const std::vector<Vertex> &vertexArray);

    double getViewWidth() const;
    double getViewHeight() const;
    void setViewSize(size_t viewWidth, size_t viewHeight);

    Vector4 getBackgroundColor() const;
    void setBackgroundColor(Vector4 backgroundColor);

    void run();
    const std::vector<Vector4>& getImage();

private:

    Vector4 getColor(std::vector<Vertex> vertexes, Vector4 position);

private:

    size_t viewWidth, viewHeight;
    Vector4 backgroundColor;

    std::vector<Vertex> vertexArray;
    std::vector<Vector4> image;

};

#endif
