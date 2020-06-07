#include <cmath>
#include <fstream>
#include "shapes.h"

std::vector<Vertex> Shapes::generateSphere(double radius, size_t ringCount, size_t edgeCount, Vector4 origin, Vector4 color) {
    std::vector<Vertex> vertexes;
    
    if (ringCount < 6 || edgeCount < 4) {
        return vertexes;
    }
    
    for (size_t half = 0; half < 2; ++half) {
        double halfSign = (half == 0 ? 1.0 : -1.0);

        std::vector< std::vector<Vertex> > rings;
        rings.resize(ringCount / 2);

        for (size_t i = 0; i < ringCount / 2; ++i) {
            double ringAlpha = M_PI * i / (double) (ringCount / 2 - 1);
            double ringRadius = radius * cos(M_PI - ringAlpha);

            for (size_t j = 0; (j < edgeCount) || (i == ringCount / 2 - 1 && j < 1); ++j) {
                double angle = 2.0 * M_PI * j / (double) (edgeCount - 1);
                auto translation = Vector4(ringRadius * sin(angle), halfSign * radius * sin(ringAlpha), ringRadius * cos(angle));
                Vertex vertex;
                vertex.origin = vertex.position = Vector4(origin.cx() + translation.cx(), origin.cy() + translation.cy(), origin.cz() + translation.cz(), 1.0);
                vertex.color = color;
                vertex.normal = translation;
                vertex.normal.normalize();
                rings[i].push_back(vertex);
            }
        }

        for (size_t i = 0; i < ringCount / 2 - 1; ++i) {
            for (size_t j = 0; j < edgeCount - 1; ++j) {
                vertexes.push_back(rings[i  ][j     % edgeCount]);
                vertexes.push_back(rings[i  ][(j+1) % edgeCount]);
                vertexes.push_back(rings[i+1][j     % rings[i+1].size()]);
                vertexes.push_back(rings[i  ][(j+1) % edgeCount]);
                vertexes.push_back(rings[i+1][(j+1) % rings[i+1].size()]);
                vertexes.push_back(rings[i+1][j     % rings[i+1].size()]);
            }
        }
    }

    return vertexes;
}

bool Shapes::saveShapeToFile(const std::string &filePath, const std::vector<Vertex> &vertexArray) {
    if (filePath.empty()) {
        return false;
    }

    std::ofstream file(filePath);

    if (file.is_open()) {
        for (size_t i = 0; i < vertexArray.size(); ++i) {
            file << vertexArray[i].position.cx() << " ";
            file << vertexArray[i].position.cy() << " ";
            file << vertexArray[i].position.cz() << " ";
            file << vertexArray[i].color.cx() << " ";
            file << vertexArray[i].color.cy() << " ";
            file << vertexArray[i].color.cz() << " ";
            file << vertexArray[i].normal.cx() << " ";
            file << vertexArray[i].normal.cy() << " ";
            file << vertexArray[i].normal.cz() << std::endl;
        }

        if (file.fail()) {
            file.close();
            return false;
        }

        file.close();
        return true;
    }

    return false;
}
