#include <algorithm>
#include <fstream>
#include <sstream>
#include "transform_engine.h"

TransformEngine::TransformEngine() {
    viewWidth = 512;
    viewHeight = 512;
    fovy = PI / 3;
    zNear = 0.1;
    zFar = 100;
}

ClipEngine& TransformEngine::getClipEngine() {
    return clipEngine;
}

double TransformEngine::getViewWidth() const {
    return viewWidth;
}

double TransformEngine::getViewHeight() const {
    return viewHeight;
}

void TransformEngine::setViewSize(double viewWidth, double viewHeight) {
    this->viewWidth = viewWidth;
    this->viewHeight = viewHeight;
}

Vector4 TransformEngine::getViewTranslation() const {
    return viewTranslation;
}

void TransformEngine::setViewTranslation(Vector4 viewTranslation) {
    this->viewTranslation = viewTranslation;
}

void TransformEngine::setViewTranslation(double x, double y, double z) {
    setViewTranslation(Vector4(x, y, z));
}

Vector4 TransformEngine::getViewRotation() const {
    return viewRotation;
}

void TransformEngine::setViewRotation(Vector4 viewRotation) {
    this->viewRotation = viewRotation;
}

void TransformEngine::setViewRotation(double angle_x, double angle_y, double angle_z) {
    setViewRotation(Vector4(angle_x, angle_y, angle_z));
}

double TransformEngine::getFovy() const {
    return fovy;
}

double TransformEngine::getZNear() const {
    return zNear;
}

double TransformEngine::getZFar() const {
    return zFar;
}

void TransformEngine::setPerspective(double fovy, double zNear, double zFar) {
    this->fovy = fovy;
    this->zNear = zNear;
    this->zFar = zFar;
}

void TransformEngine::loadModelFromVertexArray(std::vector<Vertex> &vertexArray) {
    clearModel();
    modelVertexes.insert(modelVertexes.end(), vertexArray.begin(), vertexArray.end());
}

bool TransformEngine::loadModelFromBuffer(const std::string &buffer) {
    std::vector<Vertex> vertexArray;
    std::stringstream stream(buffer);
    std::string lineBuffer;

    while (std::getline(stream, lineBuffer)) {
        // Left trim
        lineBuffer.erase(lineBuffer.begin(), std::find_if(lineBuffer.begin(), lineBuffer.end(), [](int c) {
            return !std::isspace(c);
        }));

        // Right trim
        lineBuffer.erase(std::find_if(lineBuffer.rbegin(), lineBuffer.rend(), [](int c) {
            return !std::isspace(c);
        }).base(), lineBuffer.end());

        // Empty line or comment
        if (lineBuffer.empty() || lineBuffer[0] == '#') {
            continue;
        }

        Vertex vertex;
        std::stringstream lineStream(lineBuffer);

        lineStream >> vertex.position.x();
        lineStream >> vertex.position.y();
        lineStream >> vertex.position.z();
        vertex.position.w() = 1.0;

        if (lineStream.fail()) {
            return false;
        }

        vertex.origin = vertex.position;

        lineStream >> vertex.color.x();
        lineStream >> vertex.color.y();
        lineStream >> vertex.color.z();
        vertex.color.w() = 1.0;

        if (lineStream.fail()) {
            vertex.color = Vector4(1.0, 1.0, 1.0, 1.0);
        }
        
        lineStream >> vertex.normal.x();
        lineStream >> vertex.normal.y();
        lineStream >> vertex.normal.z();
        vertex.normal.w() = 0.0;

        if (lineStream.fail()) {
            vertex.normal = Vector4(0.0, 0.0, 0.0, 0.0);
        }
        
        vertexArray.push_back(vertex);
    }

    loadModelFromVertexArray(vertexArray);

    return true;
}

bool TransformEngine::loadModelFromFile(const std::string &filePath) {
    if (filePath.empty()) {
        return true;
    }
    
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::string buffer{std::istreambuf_iterator<char>(file), {}};

        return loadModelFromBuffer(buffer);
    }

    return false;
}

void TransformEngine::clearModel() {
    modelVertexes.clear();
    transformedVertexes.clear();
}

void TransformEngine::run() {
    transformedVertexes.clear();
    
    if (modelVertexes.empty()) {
        return;
    }

    transformedVertexes.insert(transformedVertexes.end(), modelVertexes.begin(), modelVertexes.end());

    Matrix4x4 transformMatrix = getPerspectiveMatrix(fovy, zNear, zFar, viewWidth / viewHeight) *
        getRotationMatrix(viewRotation.cx(), viewRotation.cy(), viewRotation.cz()) *
        getTranslationMatrix(viewTranslation.cx(), viewTranslation.cy(), viewTranslation.cz());

    for (size_t i = 0; i < transformedVertexes.size(); ++i) {
        transformedVertexes[i].position = transformMatrix * modelVertexes[i].position;
    }

    for (size_t i = 0; i < transformedVertexes.size(); ++i) {
        transformedVertexes[i].position = transformMatrix * modelVertexes[i].position;
    }

    transformedVertexes = clipEngine.clipVertexes(transformedVertexes);

    for (size_t i = 0; i < transformedVertexes.size(); ++i) {
        for (size_t j = 0; j < 4; ++j) {
            transformedVertexes[i].position[j] /= transformedVertexes[i].position.w();
        }
    }

    for (size_t i = 0; i < transformedVertexes.size(); ++i) {
        transformedVertexes[i].position.x() = (1 + transformedVertexes[i].position.cx()) * viewWidth / 2;
        transformedVertexes[i].position.y() = (1 + transformedVertexes[i].position.cy()) * viewHeight / 2;
        transformedVertexes[i].position.z() = (1 + transformedVertexes[i].position.cz()) / 2;
    }
}

const std::vector<Vertex>& TransformEngine::getTransformedVertexes() const {
    return transformedVertexes;
}
