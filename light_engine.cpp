#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include "light_engine.h"

LightEngine::LightEngine() {
    loadMaterialFromValues();
}

Vector4 LightEngine::getViewTranslation() const {
    return viewTranslation;
}

void LightEngine::setViewTranslation(Vector4 viewTranslation) {
    this->viewTranslation = viewTranslation;
}

void LightEngine::setViewTranslation(double x, double y, double z) {
    setViewTranslation(Vector4(x, y, z));
}

void LightEngine::loadLightSourcesFromArray(const std::vector <LightSource> &lightSources, LightSource ambientLight) {
    this->lightSources.clear();
    this->lightSources.insert(this->lightSources.end(), lightSources.begin(), lightSources.end());
    this->ambientLight = ambientLight;
}

bool LightEngine::loadLightSourcesFromBuffer(const std::string &buffer) {
    std::stringstream stream(buffer);
    std::string lineBuffer;
    std::vector <LightSource> lightSources;

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

        std::stringstream lineStream(lineBuffer);
        LightSource lightSource;

        lineStream >> lightSource.intensity;

        if (lineStream.fail()) {
            return false;
        }

        lineStream >> lightSource.position.x();
        lineStream >> lightSource.position.y();
        lineStream >> lightSource.position.z();
        lightSource.position.w() = 0.0;

        if (lineStream.fail()) {
            return false;
        }

        lineStream >> lightSource.color.x();
        lineStream >> lightSource.color.y();
        lineStream >> lightSource.color.z();
        lightSource.color.w() = 1.0;

        if (lineStream.fail()) {
            lightSource.color = Vector4(1.0, 1.0, 1.0, 1.0);
        }
        
        lightSources.push_back(lightSource);
    }

    if (lightSources.empty()) {
        return true;
    }

    LightSource ambientLight = lightSources[0];
    lightSources.erase(lightSources.begin());

    loadLightSourcesFromArray(lightSources, ambientLight);

    return true;
}

bool LightEngine::loadLightSourcesFromFile(const std::string &filePath) {
    if (filePath.empty()) {
        return true;
    }

    std::ifstream file(filePath);

    if (file.is_open()) {
        std::string buffer{std::istreambuf_iterator<char>(file), {}};

        return loadLightSourcesFromBuffer(buffer);
    }

    return false;
}

void LightEngine::loadMaterialFromValues(Vector4 ambientReflection, Vector4 diffuseReflection, Vector4 specularReflection, double shininess, double suppression) {
    for (size_t i = 0; i < 4; ++i) {
        this->ambientReflection[i] = std::min(std::max(ambientReflection.c(i), 0.0), 1.0);
        this->diffuseReflection[i] = std::max(diffuseReflection.c(i), 0.0);
        this->specularReflection[i] = std::max(specularReflection.c(i), 0.0);
    }
    
    this->shininess = std::max(shininess, 0.0);
    this->suppression = std::max(suppression, 0.0);
}

bool LightEngine::loadMaterialFromBuffer(const std::string &buffer) {
    std::stringstream stream(buffer);
    std::string lineBuffer;
    std::vector<Vector4> vectorValues;
    std::vector<double> scalarValues;

    while (vectorValues.size() < 3 && std::getline(stream, lineBuffer)) {
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

        std::stringstream lineStream(lineBuffer);
        Vector4 value;

        lineStream >> value.x();
        lineStream >> value.y();
        lineStream >> value.z();

        if (lineStream.fail()) {
            return false;
        }
        
        lineStream >> value.w();

        if (lineStream.fail()) {
            value.w() = 1.0;
        }

        vectorValues.push_back(value);
    }

    if (vectorValues.size() < 3) {
        return false;
    }

    while (scalarValues.size() < 2 && std::getline(stream, lineBuffer)) {
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

        std::stringstream lineStream(lineBuffer);
        double value;

        lineStream >> value;

        if (lineStream.fail()) {
            return false;
        }
        
        scalarValues.push_back(value);
    }

    if (scalarValues.size() < 2) {
        return false;
    }

    loadMaterialFromValues(vectorValues[0], vectorValues[1], vectorValues[2], scalarValues[0], scalarValues[1]);

    return true;
}

bool LightEngine::loadMaterialFromFile(const std::string &filePath) {
    if (filePath.empty()) {
        return true;
    }

    std::ifstream file(filePath);

    if (file.is_open()) {
        std::string buffer{std::istreambuf_iterator<char>(file), {}};

        return loadMaterialFromBuffer(buffer);
    }

    return false;
}

Vector4 LightEngine::getLight(Vector4 position, Vector4 normal) const {
    Vector4 light = Vector4(1.0, 1.0, 1.0, 1.0);
    
    for (size_t i = 0; i < 4; ++i) {
        light[i] = ambientLight.intensity * ambientReflection.c(i) * ambientLight.color.c(i);
    }
    
    normal.w() = 0.0;
    normal.normalize();
    
    if (normal.getLength() == 0.0) {
        return light;
    }
    
    for (const auto& lightSource : lightSources) {
        // TODO ray casting

        Vector4 lightDirection, reflectionDirection, viewerDirection;

        for (size_t i = 0; i < 3; ++i) {
            lightDirection[i] = lightSource.position.c(i) - position.c(i);
            reflectionDirection[i] = lightDirection.c(i) - 2 * lightDirection.c(i) * normal.c(i) * normal.c(i);
            viewerDirection[i] = viewTranslation.c(i) - position.c(i);
        }

        lightDirection.normalize();
        reflectionDirection.normalize();
        viewerDirection.normalize();

        double distance = sqrt(
            pow(position.cx() - lightSource.position.cx(), 2) +
            pow(position.cy() - lightSource.position.cy(), 2) +
            pow(position.cz() - lightSource.position.cz(), 2));
        double distanceSuppression = 1 / (suppression + distance);
        double cosDiffuseAngle =
            normal.cx() * lightDirection.cx() +
            normal.cy() * lightDirection.cy() +
            normal.cz() * lightDirection.cz();
        double cosSpecularAngle =
            reflectionDirection.cx() * viewerDirection.cx() +
            reflectionDirection.cy() * viewerDirection.cy() +
            reflectionDirection.cz() * viewerDirection.cz();
        
        for (size_t i = 0; i < 4; ++i) {
            if (cosDiffuseAngle > 0.0) {
                light[i] += lightSource.intensity * distanceSuppression * diffuseReflection.c(i) * cosDiffuseAngle * lightSource.color.c(i);
            }
            if (cosSpecularAngle > 0.0) {
                light[i] += lightSource.intensity * distanceSuppression * specularReflection.c(i) * pow(cosSpecularAngle, shininess);
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        light[i] = std::min(light[i], 1.0);
    }

    return light;
}
