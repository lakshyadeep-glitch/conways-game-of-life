#include "Renderer.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace {
constexpr float SidebarWidth = 276.0F;
constexpr float GridLeft = 300.0F;
constexpr float GridTop = 78.0F;
const sf::Color Background(16, 20, 27);
const sf::Color Panel(23, 29, 38);
const sf::Color PanelLight(31, 39, 51);
const sf::Color TextPrimary(232, 239, 246);
const sf::Color TextMuted(137, 153, 170);
const sf::Color Accent(83, 220, 167);
const sf::Color AccentDark(34, 100, 79);
}

Renderer::Renderer() {
    // Use a standard Windows font if available. The app still runs if no font loads.
    const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "assets/DejaVuSans.ttf"
    };
    for (const char* path : candidates) {
        if (font_.openFromFile(path)) {
            fontLoaded_ = true;
            break;
        }
    }
}

void Renderer::drawText(sf::RenderWindow& window, const std::string& text,
                        unsigned int size, sf::Vector2f position,
                        sf::Color color) const {
    if (!fontLoaded_) {
        return;
    }
    sf::Text label(font_, text, size);
    label.setFillColor(color);
    label.setPosition(position);
    window.draw(label);
}

void Renderer::drawButton(sf::RenderWindow& window, const std::string& label,
                          sf::FloatRect bounds, bool primary, bool active) const {
    sf::RectangleShape shape(bounds.size);
    shape.setPosition(bounds.position);
    shape.setFillColor(primary ? AccentDark : (active ? sf::Color(35, 64, 57) : PanelLight));
    shape.setOutlineThickness(1.0F);
    shape.setOutlineColor(active || primary ? Accent : sf::Color(47, 59, 73));
    window.draw(shape);

    if (fontLoaded_) {
        sf::Text text(font_, label, 14);
        text.setFillColor(primary ? TextPrimary : (active ? Accent : TextPrimary));
        const sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition({
            bounds.position.x + (bounds.size.x - textBounds.size.x) / 2.0F - textBounds.position.x,
            bounds.position.y + (bounds.size.y - textBounds.size.y) / 2.0F - textBounds.position.y - 1.0F
        });
        window.draw(text);
    }
}

sf::Vector2f Renderer::gridOrigin() const { return {GridLeft, GridTop}; }

sf::Vector2f Renderer::gridAreaSize(sf::Vector2u windowSize) const {
    return {
        std::max(100.0F, static_cast<float>(windowSize.x) - GridLeft - 24.0F),
        std::max(100.0F, static_cast<float>(windowSize.y) - GridTop - 24.0F)
    };
}

bool Renderer::isInsideGrid(sf::Vector2f point, sf::Vector2u windowSize) const {
    const auto size = gridAreaSize(windowSize);
    return point.x >= GridLeft && point.y >= GridTop &&
           point.x < GridLeft + size.x && point.y < GridTop + size.y;
}

sf::Vector2i Renderer::cellFromPoint(sf::Vector2f point, int cellSize) const {
    return {
        static_cast<int>((point.x - GridLeft) / static_cast<float>(cellSize)),
        static_cast<int>((point.y - GridTop) / static_cast<float>(cellSize))
    };
}

bool Renderer::hitSpeedSlider(sf::Vector2f point) const {
    return point.x >= 28.0F && point.x <= 244.0F &&
           point.y >= 475.0F && point.y <= 510.0F;
}

float Renderer::speedFromMouseX(float mouseX) const {
    const float normalized = std::clamp((mouseX - 34.0F) / 204.0F, 0.0F, 1.0F);
    return 1.0F + normalized * 29.0F;
}

bool Renderer::hitButton(sf::Vector2f point, const std::string& id,
                         sf::Vector2u windowSize) const {
    (void)windowSize;
    sf::FloatRect bounds;
    if (id == "start") bounds = {{28, 148}, {104, 38}};
    else if (id == "step") bounds = {{140, 148}, {104, 38}};
    else if (id == "clear") bounds = {{28, 198}, {104, 38}};
    else if (id == "random") bounds = {{140, 198}, {104, 38}};
    else if (id == "patterns") bounds = {{28, 284}, {216, 40}};
    else if (id == "smaller") bounds = {{28, 380}, {102, 36}};
    else if (id == "larger") bounds = {{142, 380}, {102, 36}};
    else if (id == "menu-glider") bounds = {{340, 145}, {240, 42}};
    else if (id == "menu-pulsar") bounds = {{340, 197}, {240, 42}};
    else if (id == "menu-gun") bounds = {{340, 249}, {240, 42}};
    else if (id == "menu-close") bounds = {{340, 310}, {240, 38}};
    else return false;
    return bounds.contains(point);
}

bool Renderer::hitPattern(sf::Vector2f point, GameOfLife::Pattern& pattern) const {
    if (sf::FloatRect({340, 145}, {240, 42}).contains(point)) {
        pattern = GameOfLife::Pattern::Glider; return true;
    }
    if (sf::FloatRect({340, 197}, {240, 42}).contains(point)) {
        pattern = GameOfLife::Pattern::Pulsar; return true;
    }
    if (sf::FloatRect({340, 249}, {240, 42}).contains(point)) {
        pattern = GameOfLife::Pattern::GosperGliderGun; return true;
    }
    return false;
}

void Renderer::draw(sf::RenderWindow& window, const GameOfLife& game,
                    bool running, float speed, int cellSize,
                    bool patternMenuOpen, const std::string& statusMessage) {
    const auto windowSize = window.getSize();
    window.clear(Background);

    sf::RectangleShape sidebar({SidebarWidth, static_cast<float>(windowSize.y)});
    sidebar.setFillColor(Panel);
    window.draw(sidebar);

    drawText(window, "CELLULAR", 12, {28, 24}, TextMuted);
    drawText(window, "GAME OF LIFE", 23, {28, 43}, TextPrimary);
    drawText(window, "SIMULATION CONTROLS", 11, {28, 111}, TextMuted);

    drawButton(window, running ? "Pause" : "Start", {{28, 148}, {104, 38}}, true, running);
    drawButton(window, "Step  >|", {{140, 148}, {104, 38}});
    drawButton(window, "Clear", {{28, 198}, {104, 38}});
    drawButton(window, "Randomize", {{140, 198}, {104, 38}});

    drawText(window, "PATTERNS", 11, {28, 255}, TextMuted);
    drawButton(window, "Choose a pattern  v", {{28, 284}, {216, 40}},
               false, patternMenuOpen);

    drawText(window, "CELL SIZE", 11, {28, 351}, TextMuted);
    drawButton(window, "-  Smaller", {{28, 380}, {102, 36}});
    drawButton(window, "+  Larger", {{142, 380}, {102, 36}});
    drawText(window, std::to_string(cellSize) + " px", 12, {194, 426}, TextMuted);

    drawText(window, "SIMULATION SPEED", 11, {28, 451}, TextMuted);
    sf::RectangleShape sliderTrack({204, 4});
    sliderTrack.setPosition({34, 491});
    sliderTrack.setFillColor(sf::Color(57, 69, 82));
    window.draw(sliderTrack);
    const float sliderX = 34.0F + ((speed - 1.0F) / 29.0F) * 204.0F;
    sf::RectangleShape sliderFill({std::max(0.0F, sliderX - 34.0F), 4});
    sliderFill.setPosition({34, 491});
    sliderFill.setFillColor(Accent);
    window.draw(sliderFill);
    sf::CircleShape knob(7);
    knob.setOrigin({7, 7});
    knob.setPosition({sliderX, 493});
    knob.setFillColor(Accent);
    window.draw(knob);
    drawText(window, "1 gen/s", 11, {28, 510}, TextMuted);
    drawText(window, "30 gen/s", 11, {184, 510}, TextMuted);

    drawText(window, "LIVE STATS", 11, {28, 558}, TextMuted);
    drawText(window, "Generation", 12, {28, 587}, TextMuted);
    drawText(window, std::to_string(game.generation()), 19, {164, 579}, TextPrimary);
    drawText(window, "Population", 12, {28, 622}, TextMuted);
    drawText(window, std::to_string(game.population()), 19, {164, 614}, Accent);

    sf::RectangleShape statusDot({7, 7});
    statusDot.setPosition({29, 675});
    statusDot.setFillColor(running ? Accent : TextMuted);
    window.draw(statusDot);
    drawText(window, running ? "RUNNING" : "PAUSED", 10, {43, 670},
             running ? Accent : TextMuted);
    drawText(window, "SPACE  Start / pause", 10, {28, 710}, TextMuted);
    drawText(window, "N      Next generation", 10, {28, 730}, TextMuted);
    drawText(window, "R      Randomize", 10, {28, 750}, TextMuted);

    // Simulation viewport and subtle grid.
    sf::RectangleShape viewport(gridAreaSize(windowSize));
    viewport.setPosition(gridOrigin());
    viewport.setFillColor(sf::Color(12, 16, 22));
    viewport.setOutlineThickness(1);
    viewport.setOutlineColor(sf::Color(42, 53, 66));
    window.draw(viewport);

    for (int y = 0; y < game.rows(); ++y) {
        for (int x = 0; x < game.columns(); ++x) {
            const float px = GridLeft + static_cast<float>(x * cellSize);
            const float py = GridTop + static_cast<float>(y * cellSize);
            if (px + cellSize > static_cast<float>(windowSize.x) - 24.0F ||
                py + cellSize > static_cast<float>(windowSize.y) - 24.0F) {
                continue;
            }
            sf::RectangleShape cell({
                static_cast<float>(cellSize - 1),
                static_cast<float>(cellSize - 1)
            });
            cell.setPosition({px, py});
            if (game.isAlive(x, y)) {
                cell.setFillColor(Accent);
            } else {
                cell.setFillColor(sf::Color(22, 29, 38));
                cell.setOutlineThickness(0.5F);
                cell.setOutlineColor(sf::Color(34, 44, 56));
            }
            window.draw(cell);
        }
    }

    drawText(window, "CONWAY'S GAME OF LIFE", 11,
             {GridLeft, 31}, TextMuted);
    drawText(window, statusMessage, 11,
             {GridLeft + 230.0F, 31}, TextMuted);

    if (patternMenuOpen) {
        sf::RectangleShape overlay({static_cast<float>(windowSize.x) - GridLeft - 48.0F,
                                    static_cast<float>(windowSize.y) - 128.0F});
        overlay.setPosition({GridLeft + 24.0F, 92.0F});
        overlay.setFillColor(sf::Color(10, 14, 20, 225));
        overlay.setOutlineThickness(1);
        overlay.setOutlineColor(sf::Color(72, 92, 105));
        window.draw(overlay);
        drawText(window, "PATTERN LIBRARY", 18, {340, 108}, TextPrimary);
        drawText(window, "Choose a seed to place on the grid.", 12,
                 {340, 126}, TextMuted);
        drawButton(window, "Glider  -  moving pattern", {{340, 145}, {240, 42}});
        drawButton(window, "Pulsar  -  oscillator", {{340, 197}, {240, 42}});
        drawButton(window, "Gosper Glider Gun", {{340, 249}, {240, 42}});
        drawButton(window, "Close menu", {{340, 310}, {240, 38}});
        drawText(window, "The simulation pauses while this menu is open.",
                 11, {340, 365}, TextMuted);
    }

    window.display();
}
