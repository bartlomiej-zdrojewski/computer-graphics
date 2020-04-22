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

void RenderEngine::setVertexArray(const std::vector<Vertex> &vertexArray) {
    this->vertexArray.clear();
    this->vertexArray.insert(this->vertexArray.end(), vertexArray.begin(), vertexArray.end());
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
            } else {
                // TODO handle minor cases
            }
        }

        for (size_t x = 0; x < viewWidth; ++x) {
            std::vector<section*> rangeBuffer;

            for (size_t i = 0; i < ranges.size(); ++i) {
                if (x >= ranges[i].getLeft() && x <= ranges[i].getRight()) {
                    rangeBuffer.push_back(&ranges[i]);
                }
            }

            if (rangeBuffer.empty()) {
                image[y * viewWidth + x] = backgroundColor;
            } else if (rangeBuffer.size() == 1) {
                image[y * viewWidth + x] = getColor({
                    vertexArray[3 * rangeBuffer[0]->primitive + 0],
                    vertexArray[3 * rangeBuffer[0]->primitive + 1],
                    vertexArray[3 * rangeBuffer[0]->primitive + 2]
                }, Vector4(x, y)); ;
            } else {
                size_t frontRange = 0;
                double frontDistance = getInterpolation(rangeBuffer[0]->begin.cx(), rangeBuffer[0]->begin.cz(), rangeBuffer[0]->end.cx(), rangeBuffer[0]->end.cz(), x);

                for (size_t i = 0; i < rangeBuffer.size(); ++i) {
                    double distance = getInterpolation(rangeBuffer[i]->begin.cx(), rangeBuffer[i]->begin.cz(), rangeBuffer[i]->end.cx(), rangeBuffer[i]->end.cz(), x);
                    if (distance > frontDistance) {
                        frontRange = i;
                        frontDistance = distance;
                    }
                }

                image[y * viewWidth + x] = getColor({
                    vertexArray[3 * rangeBuffer[frontRange]->primitive + 0],
                    vertexArray[3 * rangeBuffer[frontRange]->primitive + 1],
                    vertexArray[3 * rangeBuffer[frontRange]->primitive + 2]
                }, Vector4(x, y));
            }
        }
    }
}

const std::vector<Vector4>& RenderEngine::getImage() {
    return image;
}

// TODO return gradient color
Vector4 RenderEngine::getColor(std::vector<Vertex> vertexes, Vector4 position) {
    return vertexes[0].color;
}
