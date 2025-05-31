#include <SFML/Graphics.hpp>
#include "include/TextureHolder.h"
#include <iostream>
#include <filesystem>
#include <string>

const int CELL_SIZE = 80; // Cell size (pixels)
const sf::Color LIGHT_COLOR(240, 217, 181);
const sf::Color DARK_COLOR(181, 136, 99);

TextureHolder textures;

void loadTextures() {
    for (const auto& entry : std::filesystem::directory_iterator("assets")) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            std::string filename = entry.path().stem().string();
            std::string fullPath = entry.path().string();

            textures.load(filename, fullPath);
            std::cout << "Loaded texture: " << filename << " from " << fullPath << std::endl;
        }
    }
}

void drawBoard(sf::RenderWindow& window) {
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
            cell.setFillColor((x + y) % 2 == 0 ? LIGHT_COLOR : DARK_COLOR);
            window.draw(cell);
        }
    }
}

void drawPieces(sf::RenderWindow& window, TextureHolder& textures) {
    sf::Sprite piece;
    piece.setScale(1, 1);

    float offsetX = (CELL_SIZE - piece.getGlobalBounds().width) / 2;
    float offsetY = (CELL_SIZE - piece.getGlobalBounds().height) / 2;

    piece.setTexture(textures.get("white_king"));
    piece.setPosition(0 * CELL_SIZE + offsetX, 8 * CELL_SIZE + offsetY);
    window.draw(piece);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess 10x10");

    loadTextures();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        drawBoard(window);
        drawPieces(window, textures);
        window.display();
    }

    return 0;
}
