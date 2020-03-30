#include <algorithm>
#include "clip_engine.h"

std::vector<Vector4> ClipEngine::clipVertexes(const std::vector<Vector4> &vertexes) {
    std::vector<Vector4> input, output = vertexes;
    const size_t projections[] = {3, 5, 6}; // (0,1,1), (1,0,1), (1,1,0)
    
    for (const auto& projection : projections) {
        std::vector<size_t> axises;
        
        for (size_t i = 0; i < 3; ++i) {
            if (((1 << i) & projection) != 0) {
                axises.push_back(i);
            }
        }

        input = output;
        output.clear();

        for (size_t i = 2; i < input.size(); i = i + 3) {
            bool isFullyInside = true;

            for (size_t j = 0; j < 3; ++j) {
                isFullyInside = isFullyInside && isInsideClippingSquare(input[(i-2) + j], axises);
            }

            if (isFullyInside) {
                for (size_t j = 0; j < 3; ++j) {
                    output.push_back(input[(i-2) + j]);
                }
                continue;
            }

            auto clippedPolygon = clipSutherlandHodgman({input[i-2], input[i-1], input[i]}, axises);

            // TODO Triangulate clippedPolygon and push vertexes to output
        }
    }

    return output;
}

bool ClipEngine::isInsideClippingSquare(Vector4 vertex, const std::vector<size_t> &axises) {
    double w = vertex.w();
    return vertex[axises[0]] > w && vertex[axises[0]] < -w && vertex[axises[1]] > w && vertex[axises[1]] < -w;
}

Vector4 ClipEngine::getIntersection(Vector4 vertexA, Vector4 vertexB, Vector4 planeA, Vector4 planeB, const std::vector<size_t> &axises) {
    Vector4 intersection;

    // TODO How to calculate intersection when vertexA.w() != vertexB.w() ?

    return intersection;
}

std::vector<Vector4> ClipEngine::clipSutherlandHodgman(const std::vector<Vector4> &polygon, const std::vector<size_t> &axises) {
    std::vector<Vector4> input, output = polygon;
    const size_t planes[] = {0, 1, 3, 2}; // 00, 01, 11, 10

    for (size_t i = 0; i < 4; ++i) {
        Vector4 pa, pb;
        pa[axises[0]] = planes[i] & 1;
        pa[axises[1]] = planes[i] & 2;
        pb[axises[0]] = planes[(i+1) % 4] & 1;
        pb[axises[1]] = planes[(i+1) % 4] & 2;

        input = output;
        output.clear();

        for (size_t i = 0; i < polygon.size(); ++i) {
            auto& va = polygon[i];
            auto& vb = polygon[(i+1) % polygon.size()];

            // TODO Replace isInsideClippingSquare with line-plane orientation checking function ?
            if (isInsideClippingSquare(va, axises)) {
                if (isInsideClippingSquare(vb, axises)) {
                    output.push_back(vb);
                } else {
                    output.push_back(getIntersection(va, vb, pa, pb, axises));
                }
            } else if (isInsideClippingSquare(vb, axises)) {
                output.push_back(getIntersection(va, vb, pa, pb, axises));
                output.push_back(vb);
            }
        }
    }

    return output;
}
