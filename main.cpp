#include <cmath>
#include <sstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "transform_engine.h"

const double translationStep = 0.1;
const double rotationStep = PI / 12;
const double fovyStep = PI / 12;

void displayTransformEngineStatistics(TransformEngine &transformEngine, sf::RenderWindow &window, sf::Font &font);
std::string getImageFileName();
double toDegrees(double radians);
sf::VertexArray toVertexArray(std::vector<Vector4> vertexes);

int main(int argc, char *argv[]) {
    TransformEngine transformEngine;
    sf::VertexArray vertexArray;
    sf::Font font;
    std::string modelFilePath = argc > 1 ? argv[1] : "assets/cube.txt";
    std::string fontFilePath = argc > 2 ? argv[2] : "assets/font.ttf";
    bool shouldDisplayStatistics = true;

    if (!transformEngine.loadModelFromFile(modelFilePath)) {
        std::cout << "Failed to load model \"" << modelFilePath << "\"" << std::endl;
        return 1;
    }

    if (!font.loadFromFile(fontFilePath)) {
        shouldDisplayStatistics = false;
    }

    transformEngine.setViewTranslation(0, 0, 5);
    transformEngine.run();

    vertexArray = toVertexArray(transformEngine.getTransformedVertexes());

    sf::RenderWindow window(sf::VideoMode(transformEngine.getViewWidth(), transformEngine.getViewHeight()), "Computer Graphics");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                
                transformEngine.setViewSize(event.size.width, event.size.height);
                transformEngine.run();
                
                vertexArray = toVertexArray(transformEngine.getTransformedVertexes());
            }

            if (event.type == sf::Event::KeyPressed) {
                auto viewTranslation = transformEngine.getViewTranslation();
                auto viewRotation = transformEngine.getViewRotation();
                auto fovy = transformEngine.getFovy();
                bool viewUpdated = false;

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

                if (viewUpdated) {
                    transformEngine.setViewTranslation(viewTranslation);
                    transformEngine.setViewRotation(viewRotation);
                    transformEngine.setPerspective(fovy, transformEngine.getZNear(), transformEngine.getZFar());
                    transformEngine.run();

                    vertexArray = toVertexArray(transformEngine.getTransformedVertexes());
                }
            }
        }

        window.clear();
        window.draw(vertexArray);

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
    rotationDegress.x() = toDegrees(rotationDegress.x());
    rotationDegress.y() = toDegrees(rotationDegress.y());
    rotationDegress.z() = toDegrees(rotationDegress.z());

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

sf::VertexArray toVertexArray(std::vector<Vector4> vertexes) {
    sf::VertexArray vertexArray;    
    vertexArray.setPrimitiveType(sf::PrimitiveType::Lines);

    for (size_t i = 2; i < vertexes.size(); i = i + 3) {
        for (size_t j = 0; j < 3; ++j) {
            size_t va = (i-2) + j;
            size_t vb = (i-2) + (j+1) % 3;

            vertexArray.append(sf::Vertex(sf::Vector2f(vertexes[va].x(), vertexes[va].y()), sf::Color::White));
            vertexArray.append(sf::Vertex(sf::Vector2f(vertexes[vb].x(), vertexes[vb].y()), sf::Color::White));
        }
    }
    
    return vertexArray;
}
