#include "render_engine.h"
#include <algorithm>
#include <list>
#include <map>

struct section {
    Vector4 begin;
    Vector4 end;
    size_t primitive = SIZE_MAX;

    section(Vector4 begin, Vector4 end, size_t primitive = SIZE_MAX) {
        this->begin = begin;
        this->end = end;
        this->primitive = primitive;
    }

    bool operator<(const section& right) const {
        return getTop() < right.getTop();
    }

    double getTop() const {
        return std::min(begin.cy(), end.cy());
    }

    double getBottom() const {
        return std::max(begin.cy(), end.cy());
    }

    double getLeft() const {
        return std::min(begin.cx(), end.cx());
    }

    double getRight() const {
        return std::max(begin.cx(), end.cx());
    }
};

double getInterpolation(double x0, double y0, double x1, double y1, double x) {
    if (std::abs(x1 - x0) < 0.0001) {
        return (y0 + y1) / 2;
    }

    return (y0 * (x1 - x) + y1 * (x - x0)) / (x1 - x0);
}

Vector4 getIntersection(const section& s, double y) {
    double x = getInterpolation(s.begin.cy(), s.begin.cx(), s.end.cy(), s.end.cx(), y);
    double z = getInterpolation(s.begin.cx(), s.begin.cz(), s.end.cx(), s.end.cz(), x);
    
    return Vector4(x, y, z);
}

RenderEngine::RenderEngine() {
    viewWidth = 512;
    viewHeight = 512;
    backgroundColor = Vector4(0, 0, 0, 1);
}

LightEngine& RenderEngine::getLightEngine() {
    return lightEngine;
}

double RenderEngine::getViewWidth() const {
    return viewWidth;
}

double RenderEngine::getViewHeight() const {
    return viewHeight;
}

void RenderEngine::setViewSize(size_t viewWidth, size_t viewHeight) {
    this->viewWidth = viewWidth;
    this->viewHeight = viewHeight;
}

Vector4 RenderEngine::getBackgroundColor() const {
    return backgroundColor;
}

void RenderEngine::setBackgroundColor(Vector4 backgroundColor) {
    this->backgroundColor = backgroundColor;
}

void RenderEngine::setVertexArray(const std::vector<Vertex> &vertexArray) {
    this->vertexArray.clear();
    this->vertexArray.insert(this->vertexArray.end(), vertexArray.begin(), vertexArray.end());
}

void RenderEngine::run() {
    size_t nextEdge = 0;
    std::vector<section> edges;
    std::list<section*> edgeBuffer;

    image.resize(viewWidth * viewHeight);

    for (size_t i = 2; i < vertexArray.size(); i = i + 3) {
        for (size_t j = 0; j < 3; ++j) {
            const size_t va = (i-2) + j;
            const size_t vb = (i-2) + (j+1) % 3;
            edges.emplace_back(vertexArray[va].position, vertexArray[vb].position, i/3);
        }
    }
    
    std::sort(edges.begin(), edges.end());

    for (size_t y = 0; y < viewHeight; ++y) {
        std::map<size_t, std::vector<section*> > edgeMap;
        std::vector<section> ranges;
        
        while (nextEdge < edges.size() && edges[nextEdge].getTop() <= y) {
            edgeBuffer.push_back(&edges[nextEdge]);
            ++nextEdge;
        }
        
        for (auto it = edgeBuffer.begin(); it != edgeBuffer.end();) {
            if ((*it)->getBottom() < y) {
                it = edgeBuffer.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = edgeBuffer.begin(); it != edgeBuffer.end(); ++it) {
            edgeMap[(*it)->primitive].push_back(*it);
        }

        for (auto it = edgeMap.begin(); it != edgeMap.end(); ++it) {
            if (it->second.size() == 2) {
                const auto ia = getIntersection(*(it->second[0]), y);
                const auto ib = getIntersection(*(it->second[1]), y);
                ranges.emplace_back(ia, ib, it->first);
            } else if (!it->second.empty()) {
                // TODO handle minor cases
            }
        }

        // TODO sort ranges and update rangeBuffer as you go

        for (size_t x = 0; x < viewWidth; ++x) {
            std::vector<section*> rangeBuffer;

            // TODO optimize
            for (size_t i = 0; i < ranges.size(); ++i) {
                if (x >= ranges[i].getLeft() && x <= ranges[i].getRight()) {
                    rangeBuffer.push_back(&ranges[i]);
                }
            }

            if (rangeBuffer.empty()) {
                image[y * viewWidth + x] = backgroundColor;
            } else {
                size_t frontRange = 0;
                double frontDistance = getInterpolation(rangeBuffer[0]->begin.cx(), rangeBuffer[0]->begin.cz(), rangeBuffer[0]->end.cx(), rangeBuffer[0]->end.cz(), x);

                for (size_t i = 1; i < rangeBuffer.size(); ++i) {
                    double distance = getInterpolation(rangeBuffer[i]->begin.cx(), rangeBuffer[i]->begin.cz(), rangeBuffer[i]->end.cx(), rangeBuffer[i]->end.cz(), x);
                    if (distance > frontDistance) {
                        frontRange = i;
                        frontDistance = distance;
                    }
                }

                const auto color = getGradientColor({
                        vertexArray[3 * rangeBuffer[frontRange]->primitive + 0],
                        vertexArray[3 * rangeBuffer[frontRange]->primitive + 1],
                        vertexArray[3 * rangeBuffer[frontRange]->primitive + 2]
                    }, Vector4(x, y));

                const auto light = getLight({
                        vertexArray[3 * rangeBuffer[frontRange]->primitive + 0],
                        vertexArray[3 * rangeBuffer[frontRange]->primitive + 1],
                        vertexArray[3 * rangeBuffer[frontRange]->primitive + 2]
                    }, Vector4(x, y));
                
                image[y * viewWidth + x] = Vector4(
                    light.cx() * color.cx(),
                    light.cy() * color.cy(),
                    light.cz() * color.cz(),
                    light.cw() * color.cw());
            }
        }
    }
}

const std::vector<Vector4>& RenderEngine::getImage() {
    return image;
}

Vector4 RenderEngine::getGradientColor(const std::vector<Vertex> &primitive, Vector4 position) {
    Vector4 gradientColor;
    std::vector <Vertex> referenceVertexes;

    for (size_t i = 0; i < primitive.size(); ++i) {
        auto topIndex = i;
        auto bottomIndex = (i+1) % primitive.size();
        auto top = primitive[topIndex].position.cy();
        auto bottom = primitive[(i+1) % primitive.size()].position.cy();

        if (top > bottom) {
            std::swap(top, bottom);
            std::swap(topIndex, bottomIndex);
        }

        if (position.cy() >= top && position.cy() <= bottom) {
            Vertex vertex;
            vertex.position.x() = getInterpolation(
                primitive[i].position.cy(), primitive[i].position.cx(),
                primitive[(i+1) % primitive.size()].position.cy(), primitive[(i+1) % primitive.size()].position.cx(),
                position.cy());
            vertex.position.y() = position.cy();

            double alpha = (position.cy() - top) / (bottom - top);
            alpha = std::min(std::max(alpha, 0.0), 1.0);

            for (size_t i = 0; i < 4; ++i) {
                vertex.color[i] = primitive[topIndex].color.c(i) + alpha * (primitive[bottomIndex].color.c(i) - primitive[topIndex].color.c(i));
            }

            referenceVertexes.push_back(vertex);
        }
    }
    
    if (referenceVertexes.size() != 2) {
        if (referenceVertexes.empty()) {
            return primitive[0].color;
        } else {
            return referenceVertexes[0].color;
        }
    }

    auto leftIndex = 0;
    auto rightIndex = 1;
    auto left = referenceVertexes[leftIndex].position.cx();
    auto right = referenceVertexes[rightIndex].position.cx();

    if (left > right) {
        std::swap(left, right);
        std::swap(leftIndex, rightIndex);
    }

    double alpha = (position.cx() - left) / (right - left);
    alpha = std::min(std::max(alpha, 0.0), 1.0);

    for (size_t i = 0; i < 4; ++i) {
        gradientColor[i] = referenceVertexes[leftIndex].color.c(i) + alpha * (referenceVertexes[rightIndex].color.c(i) - referenceVertexes[leftIndex].color.c(i));
    }

    return gradientColor;
}

Vector4 RenderEngine::getLight(const std::vector<Vertex> &primitive, Vector4 position) {
    std::vector<Vertex> primitiveLight;
    
    for (size_t i = 0; i < primitive.size(); ++i) {
        Vertex vertex;
        vertex.position = primitive[i].position;
        vertex.color = lightEngine.getLight(primitive[i].origin, primitive[i].normal);
        primitiveLight.push_back(vertex);
    }

    return getGradientColor(primitiveLight, position);
}
