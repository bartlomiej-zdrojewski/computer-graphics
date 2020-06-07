#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "vertex.h"

class Shapes {

public:

    static std::vector<Vertex> generateSphere(double radius, size_t ringCount, size_t edgeCount,
        Vector4 origin = Vector4(), Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0));

    static bool saveShapeToFile(const std::string &filePath, const std::vector<Vertex> &vertexArray);

private:

    Shapes() {};

};

#endif
