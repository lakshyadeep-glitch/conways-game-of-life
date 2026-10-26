#pragma once

#include "GameOfLife.hpp"
#include "Renderer.hpp"

#include <SFML/Graphics.hpp>
#include <string>

class Application {
public:
    Application();
    int run();

private:
    sf::RenderWindow window_;
    GameOfLife game_;
    Renderer renderer_;
    bool running_ = false;
    bool patternMenuOpen_ = false;
    bool draggingSpeed_ = false;
    float speed_ = 6.0F; // generations per second
    int cellSize_ = 13;
    sf::Clock simulationClock_;
    std::string statusMessage_ = "Click cells to draw your own pattern";

    void processEvents();
    void update();
    void render();
    void fitGridToWindow();
    void handleClick(sf::Vector2f point);
};
