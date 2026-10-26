#include "Application.hpp"

#include <algorithm>
#include <cmath>

Application::Application()
    : window_(sf::VideoMode({1280U, 800U}), "Conway's Game of Life",
              sf::Style::Default),
      game_(64, 40) {
    window_.setFramerateLimit(60);
    fitGridToWindow();
}

void Application::fitGridToWindow() {
    const auto size = window_.getSize();
    const int availableWidth = std::max(100, static_cast<int>(size.x) - 324);
    const int availableHeight = std::max(100, static_cast<int>(size.y) - 102);
    const int columns = std::max(1, availableWidth / cellSize_);
    const int rows = std::max(1, availableHeight / cellSize_);
    game_.resize(columns, rows);
}

int Application::run() {
    while (window_.isOpen()) {
        processEvents();
        update();
        render();
    }
    return 0;
}

void Application::processEvents() {
    while (const std::optional event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            (void)resized;
            fitGridToWindow();
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (patternMenuOpen_) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    patternMenuOpen_ = false;
                }
                continue;
            }
            if (key->code == sf::Keyboard::Key::Space) {
                running_ = !running_;
                statusMessage_ = running_ ? "Simulation is running" : "Simulation paused";
            } else if (key->code == sf::Keyboard::Key::N) {
                game_.step();
                running_ = false;
                statusMessage_ = "Advanced by one generation";
            } else if (key->code == sf::Keyboard::Key::R) {
                game_.randomize();
                running_ = false;
                statusMessage_ = "Randomized the grid";
            } else if (key->code == sf::Keyboard::Key::C) {
                game_.clear();
                running_ = false;
                statusMessage_ = "Grid cleared";
            } else if (key->code == sf::Keyboard::Key::Escape) {
                running_ = false;
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button != sf::Mouse::Button::Left) {
                continue;
            }
            const sf::Vector2f point(static_cast<float>(mouse->position.x),
                                     static_cast<float>(mouse->position.y));
            if (renderer_.hitSpeedSlider(point)) {
                draggingSpeed_ = true;
                speed_ = renderer_.speedFromMouseX(point.x);
            } else {
                handleClick(point);
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouse->button == sf::Mouse::Button::Left) {
                draggingSpeed_ = false;
            }
        } else if (event->is<sf::Event::MouseMoved>()) {
            if (draggingSpeed_) {
                const auto* moved = event->getIf<sf::Event::MouseMoved>();
                speed_ = renderer_.speedFromMouseX(static_cast<float>(moved->position.x));
            }
        }
    }
}

void Application::handleClick(sf::Vector2f point) {
    const auto windowSize = window_.getSize();

    if (patternMenuOpen_) {
        GameOfLife::Pattern selected{};
        if (renderer_.hitPattern(point, selected)) {
            game_.placePattern(selected, game_.columns() / 2, game_.rows() / 2);
            patternMenuOpen_ = false;
            running_ = false;
            statusMessage_ = "Pattern placed";
        } else if (renderer_.hitButton(point, "menu-close", windowSize) ||
                   point.x < 300.0F) {
            patternMenuOpen_ = false;
        }
        return;
    }

    if (renderer_.hitButton(point, "start", windowSize)) {
        running_ = !running_;
        statusMessage_ = running_ ? "Simulation is running" : "Simulation paused";
    } else if (renderer_.hitButton(point, "step", windowSize)) {
        game_.step();
        running_ = false;
        statusMessage_ = "Advanced by one generation";
    } else if (renderer_.hitButton(point, "clear", windowSize)) {
        game_.clear();
        running_ = false;
        statusMessage_ = "Grid cleared";
    } else if (renderer_.hitButton(point, "random", windowSize)) {
        game_.randomize();
        running_ = false;
        statusMessage_ = "Randomized the grid";
    } else if (renderer_.hitButton(point, "patterns", windowSize)) {
        running_ = false;
        patternMenuOpen_ = true;
        statusMessage_ = "Choose a pattern";
    } else if (renderer_.hitButton(point, "smaller", windowSize)) {
        cellSize_ = std::max(6, cellSize_ - 1);
        fitGridToWindow();
        statusMessage_ = "Cell size decreased";
    } else if (renderer_.hitButton(point, "larger", windowSize)) {
        cellSize_ = std::min(20, cellSize_ + 1);
        fitGridToWindow();
        statusMessage_ = "Cell size increased";
    } else if (renderer_.isInsideGrid(point, windowSize)) {
        const auto cell = renderer_.cellFromPoint(point, cellSize_);
        game_.toggleCell(cell.x, cell.y);
        statusMessage_ = "Click cells to draw your own pattern";
    }
}

void Application::update() {
    if (patternMenuOpen_) {
        simulationClock_.restart();
        return;
    }

    const float elapsed = simulationClock_.restart().asSeconds();
    if (running_) {
        // Fixed-step accumulator: generation speed is independent of frame rate.
        static float accumulator = 0.0F;
        accumulator += std::min(elapsed, 0.25F);
        const float interval = 1.0F / speed_;
        while (accumulator >= interval) {
            game_.step();
            accumulator -= interval;
        }
    }
}

void Application::render() {
    renderer_.draw(window_, game_, running_, speed_, cellSize_,
                   patternMenuOpen_, statusMessage_);
}
