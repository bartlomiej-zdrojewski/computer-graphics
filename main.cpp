#include <cmath>
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "transform_engine.h"
#include "render_engine.h"

enum RenderModes {
    Points,
    Lines,
    Polygons
};

const double translationStep = 0.1;
const double rotationStep = PI / 12;
const double fovyStep = PI / 12;
const auto renderMode = RenderModes::Polygons;

void displayTransformEngineStatistics(TransformEngine &transformEngine, sf::RenderWindow &window, sf::Font &font);
std::string getImageFileName();
double toDegrees(double radians);
sf::VertexArray toVertexArray(const std::vector<Vertex>& vertexes);
sf::Texture toTexture(const std::vector<Vector4>& image, size_t width, size_t height);

int main(int argc, char *argv[]) {
    TransformEngine transformEngine;
    RenderEngine renderEngine;
    sf::VertexArray vertexArray;
    sf::Texture texture;
    sf::Font font;
    std::string modelFilePath = argc > 1 ? argv[1] : "assets/models/color/triangle.txt";
    std::string materialFilePath = argc > 2 ? argv[2] : "";
    std::string lightSourcesFilePath = argc > 3 ? argv[3] : "";
    std::string fontFilePath = argc > 4 ? argv[4] : "assets/fonts/inconsolata.ttf";
    bool shouldDisplayStatistics = true;

    if (!transformEngine.loadModelFromFile(modelFilePath)) {
        std::cout << "Failed to load model \"" << modelFilePath << "\"" << std::endl;
        return 1;
    }

    if (!renderEngine.getLightEngine().loadMaterialFromFile(materialFilePath)) {
        std::cout << "Failed to load material \"" << materialFilePath << "\"" << std::endl;
        return 2;
    }

    if (!renderEngine.getLightEngine().loadLightSourcesFromFile(lightSourcesFilePath)) {
        std::cout << "Failed to load light sources \"" << lightSourcesFilePath << "\"" << std::endl;
        return 3;
    }

    if (!font.loadFromFile(fontFilePath)) {
        shouldDisplayStatistics = false;
    }

    // Moves camera to (0,0,-5). It will be pointed at (0,0,0), so keep in mind that X axis will be inverted.
    transformEngine.setViewTranslation(0, 0, 5);
    transformEngine.run();
    
    renderEngine.setVertexArray(transformEngine.getTransformedVertexes());
    renderEngine.setViewSize(transformEngine.getViewWidth(), transformEngine.getViewHeight());
    renderEngine.getLightEngine().setViewTranslation(transformEngine.getViewTranslation());
    renderEngine.run();
    
    if (renderMode == RenderModes::Polygons) {
        vertexArray = toVertexArray(transformEngine.getTransformedVertexes());
    } else {
        texture = toTexture(renderEngine.getImage(), renderEngine.getViewWidth(), renderEngine.getViewHeight());
    }

    sf::RenderWindow window(sf::VideoMode(transformEngine.getViewWidth(), transformEngine.getViewHeight()), "Computer Graphics");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        bool viewUpdated = false;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                transformEngine.setViewSize(event.size.width, event.size.height);
                renderEngine.setViewSize(transformEngine.getViewWidth(), transformEngine.getViewHeight());
                viewUpdated = true;
            }

            if (event.type == sf::Event::KeyPressed) {
                auto viewTranslation = transformEngine.getViewTranslation();
                auto viewRotation = transformEngine.getViewRotation();
                auto fovy = transformEngine.getFovy();

                if (event.key.code == sf::Keyboard::W) {
                    viewTranslation.x() += translationStep * cos(viewRotation.y() - PI / 2);
                    viewTranslation.z() += translationStep * sin(viewRotation.y() - PI / 2);
                    viewUpdated = true;
                }
                else if (event.key.code == sf::Keyboard::S) {
                    viewTranslation.x() -= translationStep * cos(viewRotation.y() - PI / 2);
                    viewTranslation.z() -= translationStep * sin(viewRotation.y() - PI / 2);
                    viewUpdated = true;
                }
                else if (event.key.code == sf::Keyboard::A) {
                    viewRotation.y() -= rotationStep;
                    viewUpdated = true;
                }
                else if (event.key.code == sf::Keyboard::D) {
                    viewRotation.y() += rotationStep;
                    viewUpdated = true;
                } else if (event.key.code == sf::Keyboard::Q) {
                    fovy -= fovyStep;
                    if (fovy < fovyStep) {
                        fovy = fovyStep;
                    }
                    viewUpdated = true;
                } else if (event.key.code == sf::Keyboard::E) {
                    fovy += fovyStep;
                    if (fovy > PI - fovyStep) {
                        fovy = PI - fovyStep;
                    }
                    viewUpdated = true;
                } else if (event.key.code == sf::Keyboard::Z) {
                    shouldDisplayStatistics = !shouldDisplayStatistics;
                } else if (event.key.code == sf::Keyboard::X) {
                    sf::Texture texture;
                    texture.create(window.getSize().x, window.getSize().y);
                    texture.update(window);
                    auto image = texture.copyToImage();
                    image.saveToFile(getImageFileName());
                } else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }

                transformEngine.setViewTranslation(viewTranslation);
                transformEngine.setViewRotation(viewRotation);
                transformEngine.setPerspective(fovy, transformEngine.getZNear(), transformEngine.getZFar());
            }
        }

        if (viewUpdated) {
            transformEngine.run();

            if (renderMode == RenderModes::Polygons) {
                renderEngine.setVertexArray(transformEngine.getTransformedVertexes());
                renderEngine.getLightEngine().setViewTranslation(transformEngine.getViewTranslation());
                renderEngine.run();
                texture = toTexture(renderEngine.getImage(), renderEngine.getViewWidth(), renderEngine.getViewHeight());
            } else {
                vertexArray = toVertexArray(transformEngine.getTransformedVertexes());
            }
        }

        window.clear();
        
        if (renderMode == RenderModes::Polygons) {
            window.draw(sf::Sprite(texture));
        } else {
            window.draw(vertexArray);
        }

        if (shouldDisplayStatistics) {
            displayTransformEngineStatistics(transformEngine, window, font);
        }
        
        window.display();
    }

    return 0;
}

void displayTransformEngineStatistics(TransformEngine &transformEngine, sf::RenderWindow &window, sf::Font &font) {
    std::stringstream stream;
    std::string buffer;

    auto translation = transformEngine.getViewTranslation();
    auto rotationDegress = transformEngine.getViewRotation();
    rotationDegress.x() = toDegrees(rotationDegress.cx());
    rotationDegress.y() = toDegrees(rotationDegress.cy());
    rotationDegress.z() = toDegrees(rotationDegress.cz());

    stream.precision(2);
    stream << std::fixed
           << "Translation: " << translation.x() << ", " << translation.y() << ", " << translation.z() << std::endl
           << "Rotation: " << rotationDegress.x() << ", " << rotationDegress.y() << ", " << rotationDegress.z() << " degrees " << std::endl
           << "Field of view: " << toDegrees(transformEngine.getFovy()) << " degrees";

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);

    sf::Vector2f textPosition(4.f, 0.f);
    float lineSpacing = 16.f;

    while (std::getline(stream, buffer)) {
        text.setString(buffer);
        text.setPosition(textPosition);
        window.draw(text);

        textPosition.y += lineSpacing;
    }
}

std::string getImageFileName() {
    auto timestamp = static_cast<long int> (time(nullptr));
    
    return "image_" + std::to_string(timestamp) + ".png";
}

double toDegrees(double radians) {
    double degrees = fmod(radians * 180 / PI, 360);

    if (degrees < -180) {
        degrees += 360;
    } else if (degrees > 180) {
        degrees -= 360;
    }

    return degrees;
}

sf::VertexArray toVertexArray(const std::vector<Vertex>& vertexes) {
    sf::VertexArray vertexArray;

    if (renderMode == RenderModes::Points) {
        vertexArray.resize(vertexes.size());
        vertexArray.setPrimitiveType(sf::PrimitiveType::Points);
        for (size_t i = 0; i < vertexes.size(); ++i) {
            sf::Vector2f position(vertexes[i].position.cx(), vertexes[i].position.cy());
            sf::Color color(vertexes[i].color.cx() * 255, vertexes[i].color.cy() * 255, vertexes[i].color.cz() * 255, vertexes[i].color.cw() * 255);
            vertexArray[i] = sf::Vertex(position, color);
        }
    } else if (renderMode == RenderModes::Lines) {
        vertexArray.setPrimitiveType(sf::PrimitiveType::Lines);
        for (size_t i = 2; i < vertexes.size(); i = i + 3) {
            for (size_t j = 0; j < 3; ++j) {
                const size_t va = (i-2) + j;
                const size_t vb = (i-2) + (j+1) % 3;
                vertexArray.append(sf::Vertex(sf::Vector2f(vertexes[va].position.cx(), vertexes[va].position.cy()),
                    sf::Color(vertexes[va].color.cx() * 255, vertexes[va].color.cy() * 255, vertexes[va].color.cz() * 255, vertexes[va].color.cw() * 255)));
                vertexArray.append(sf::Vertex(sf::Vector2f(vertexes[vb].position.cx(), vertexes[vb].position.cy()),
                    sf::Color(vertexes[vb].color.cx() * 255, vertexes[vb].color.cy() * 255, vertexes[vb].color.cz() * 255, vertexes[vb].color.cw() * 255)));
            }
        }
    }
    
    return vertexArray;
}

sf::Texture toTexture(const std::vector<Vector4>& image, size_t width, size_t height) {
    sf::Image buffer;
    sf::Texture texture;
    
    buffer.create(width, height);
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            Vector4 pixel = image[y * width + x];
            buffer.setPixel(x, y, sf::Color(pixel.cx() * 255, pixel.cy() * 255, pixel.cz() * 255, pixel.cw() * 255));
        }
    }

    texture.loadFromImage(buffer);

    return texture;
}
