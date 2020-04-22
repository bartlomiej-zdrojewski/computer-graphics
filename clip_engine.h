#ifndef CLIP_ENGINE_H
#define CLIP_ENGINE_H

#include <vector>
#include "vertex.h"

class ClipEngine {

public:

    std::vector<Vertex> clipVertexes(const std::vector<Vertex> &vertexes) const;

private:

    bool isInsideClippingSquare(Vector4 vertex, const std::vector<size_t> &axises) const;
    Vector4 getIntersection(Vector4 vertexA, Vector4 vertexB, Vector4 planeA, Vector4 planeB, const std::vector<size_t> &axises) const;

    std::vector<Vector4> clipSutherlandHodgman(const std::vector<Vector4> &polygon, const std::vector<size_t> &axises) const;

};

#endif