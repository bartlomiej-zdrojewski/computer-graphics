#ifndef CLIP_ENGINE_H
#define CLIP_ENGINE_H

#include <vector>
#include "vector4.h"

class ClipEngine {

public:

    std::vector<Vector4> clipVertexes(const std::vector<Vector4> &vertexes);

private:

    bool isInsideClippingSquare(Vector4 vertex, const std::vector<size_t> &axises);
    Vector4 getIntersection(Vector4 vertexA, Vector4 vertexB, Vector4 planeA, Vector4 planeB, const std::vector<size_t> &axises);

    std::vector<Vector4> clipSutherlandHodgman(const std::vector<Vector4> &polygon, const std::vector<size_t> &axises);

};

#endif