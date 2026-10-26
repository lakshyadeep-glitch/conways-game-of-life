#pragma once

#include "GameOfLife.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Renderer {
public:
    Renderer();

    void draw(sf::RenderWindow& window,
              const GameOfLife& game,
              bool running,
              float speed,
              int cellSize,
              bool patternMenuOpen,
              const std::string& statusMessage);

    // UI hit-testing helpers. Coordinates are in window pixels.
    bool hitButton(sf::Vector2f point, const std::string& buttonId,
                   sf::Vector2u windowSize) const;
    bool hitSpeedSlider(sf::Vector2f point) const;
    float speedFromMouseX(float mouseX) const;
    bool isInsideGrid(sf::Vector2f point, sf::Vector2u windowSize) const;
    sf::Vector2i cellFromPoint(sf::Vector2f point, int cellSize) const;
    sf::Vector2f gridOrigin() const;
    sf::Vector2f gridAreaSize(sf::Vector2u windowSize) const;
    bool hitPattern(sf::Vector2f point, GameOfLife::Pattern& pattern) const;

private:
    sf::Font font_;
    bool fontLoaded_ = false;

    void drawText(sf::RenderWindow& window, const std::string& text,
                  unsigned int size, sf::Vector2f position,
                  sf::Color color) const;
    void drawButton(sf::RenderWindow& window, const std::string& label,
                    sf::FloatRect bounds, bool primary = false,
                    bool active = false) const;
};
