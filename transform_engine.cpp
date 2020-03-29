#include <sstream>
#include <fstream>
#include <algorithm>
#include "transform_engine.h"

TransformEngine::TransformEngine() {
    viewWidth = 512;
    viewHeight = 512;
    fovy = PI / 3;
    zNear = 0.1;
    zFar = 100;
}

void TransformEngine::loadModelFromVertexArray(std::vector<Vector4> &vertexArray) {
    modelVertexes.insert(modelVertexes.end(), vertexArray.begin(), vertexArray.end());
}

bool TransformEngine::loadModelFromBuffer(const std::string &buffer) {
    std::vector<Vector4> vertexArray;
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

        Vector4 vertex;
        std::stringstream lineStream(lineBuffer);

        lineStream >> vertex.x();
        lineStream >> vertex.y();
        lineStream >> vertex.z();
        vertex.w() = 1;

        if (lineStream.fail()) {
            return false;
        }

        vertexArray.push_back(vertex);
    }

    loadModelFromVertexArray(vertexArray);

    return true;
}

bool TransformEngine::loadModelFromFile(const std::string &filePath) {
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::string buffer{std::istreambuf_iterator<char>(file), {}};

        return loadModelFromBuffer(buffer);
    }

    return false;
}

void TransformEngine::clearModel() {
    modelVertexes.clear();
}

double TransformEngine::getViewWidth() {
    return viewWidth;
}

double TransformEngine::getViewHeight() {
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

void TransformEngine::run() {
    if (modelVertexes.empty()) {
        return;
    }

    Matrix4x4 transformMatrix = getPerspectiveMatrix(fovy, zNear, zFar, viewWidth / viewHeight) *
        getRotationMatrix(viewRotation.x(), viewRotation.y(), viewRotation.z()) *
        getTranslationMatrix(viewTranslation.x(), viewTranslation.y(), viewTranslation.z());

    transformedVertexes.clear();
    transformedVertexes.resize(modelVertexes.size());

    for (size_t i = 0; i < modelVertexes.size(); ++i) {
        transformedVertexes[i] = transformMatrix * modelVertexes[i];
    }

    for (size_t i = 0; i < modelVertexes.size(); ++i) {
        transformedVertexes[i] = transformMatrix * modelVertexes[i];
    }

    transformedVertexes = clipEngine.clipVertexes(transformedVertexes);

    for (size_t i = 0; i < modelVertexes.size(); ++i) {
        for (size_t j = 0; j < 4; ++j) {
            transformedVertexes[i][j] /= transformedVertexes[i].w();
        }
    }

    for (size_t i = 0; i < modelVertexes.size(); ++i) {
        transformedVertexes[i].x() = (1 + transformedVertexes[i].x()) * viewWidth / 2;
        transformedVertexes[i].y() = (1 + transformedVertexes[i].y()) * viewHeight / 2;
        transformedVertexes[i].z() = (1 + transformedVertexes[i].z()) * (zFar - zNear) / 2;
    }
}

std::vector<Vector4> TransformEngine::getTransformedVertexes() const {
    return transformedVertexes;
}
