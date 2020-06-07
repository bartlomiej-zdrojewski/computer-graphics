#ifndef LIGHT_ENGINE_H
#define LIGHT_ENGINE_H

#include <vector>
#include "light_source.h"

class LightEngine {

public:

    LightEngine();

    Vector4 getViewTranslation() const;
    void setViewTranslation(Vector4 viewTranslation);
    void setViewTranslation(double x, double y, double z);

    void loadLightSourcesFromArray(const std::vector <LightSource> &lightSources, LightSource ambientLight = LightSource());
    bool loadLightSourcesFromBuffer(const std::string &buffer);
    bool loadLightSourcesFromFile(const std::string &filePath);

    void loadMaterialFromValues(
        Vector4 ambientReflection = Vector4(1.0, 1.0, 1.0, 1.0),
        Vector4 diffuseReflection = Vector4(0.0, 0.0, 0.0, 1.0),
        Vector4 specularReflection = Vector4(0.0, 0.0, 0.0, 1.0),
        double shininess = 1.0, double suppression = 1.0);
    bool loadMaterialFromBuffer(const std::string &buffer);
    bool loadMaterialFromFile(const std::string &filePath);

    Vector4 getLight(Vector4 position, Vector4 normal) const;

private:

    Vector4 viewTranslation;
    Vector4 ambientReflection;
    Vector4 diffuseReflection;
    Vector4 specularReflection;
    double shininess;
    double suppression;

    LightSource ambientLight;
    std::vector <LightSource> lightSources;

};

#endif
