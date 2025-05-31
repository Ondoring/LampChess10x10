#include <SFML/Graphics.hpp>
#include "include/TextureHolder.h"
#include "include/Piece.h"
#include "include/Pawn.h"
#include "include/Bishop.h"
#include "include/King.h"
#include "include/Knight.h"
#include "include/Queen.h"
#include "include/Rook.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

const int CELL_SIZE = 80; // Cell size (pixels)
const sf::Color LIGHT_COLOR(240, 217, 181);
const sf::Color DARK_COLOR(181, 136, 99);

TextureHolder textures;
std::vector<std::vector<Piece*>> board(10, std::vector<Piece*>(10, nullptr));
sf::Vector2i selectedPos(-1, -1); // Выбранная клетка
std::vector<sf::Vector2i> possibleMoves; // Возможные ходы

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

void initBoard() {
    // Очистка доски (на случай повторной инициализации)
    for (auto& row : board) {
        for (auto& piece : row) {
            delete piece;
            piece = nullptr;
        }
    }

    // Пешки
    for (int x = 1; x < 9; ++x) {
        // Белые пешки (линия 7)
        board[x][7] = new Pawn(PieceColor::White);
        board[x][7]->setSprite(textures.get("white_pawn"));

        // Чёрные пешки (линия 2)
        board[x][2] = new Pawn(PieceColor::Black);
        board[x][2]->setSprite(textures.get("black_pawn"));
    }

    board[1][8] = new Rook(PieceColor::White);
    board[1][8]->setSprite(textures.get("white_rook"));
    board[8][8] = new Rook(PieceColor::White);
    board[8][8]->setSprite(textures.get("white_rook"));

    board[1][1] = new Rook(PieceColor::Black);
    board[1][1]->setSprite(textures.get("black_rook"));
    board[8][1] = new Rook(PieceColor::Black);
    board[8][1]->setSprite(textures.get("black_rook"));

    board[2][8] = new Knight(PieceColor::White);
    board[2][8]->setSprite(textures.get("white_knight"));
    board[7][8] = new Knight(PieceColor::White);
    board[7][8]->setSprite(textures.get("white_knight"));

    board[2][1] = new Knight(PieceColor::Black);
    board[2][1]->setSprite(textures.get("black_knight"));
    board[7][1] = new Knight(PieceColor::Black);
    board[7][1]->setSprite(textures.get("black_knight"));

    board[3][8] = new Bishop(PieceColor::White);
    board[3][8]->setSprite(textures.get("white_bishop"));
    board[6][8] = new Bishop(PieceColor::White);
    board[6][8]->setSprite(textures.get("white_bishop"));

    board[3][1] = new Bishop(PieceColor::Black);
    board[3][1]->setSprite(textures.get("black_bishop"));
    board[6][1] = new Bishop(PieceColor::Black);
    board[6][1]->setSprite(textures.get("black_bishop"));

    board[4][8] = new Queen(PieceColor::White);
    board[4][8]->setSprite(textures.get("white_queen"));
    board[4][1] = new Queen(PieceColor::Black);
    board[4][1]->setSprite(textures.get("black_queen"));

    board[5][8] = new King(PieceColor::White);
    board[5][8]->setSprite(textures.get("white_king"));
    board[5][1] = new King(PieceColor::Black);
    board[5][1]->setSprite(textures.get("black_king"));
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

void drawPieces(sf::RenderWindow& window) {
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (board[x][y]) {
                sf::Sprite piece = board[x][y]->getSprite();
                // Центрирование спрайта в клетке
                piece.setPosition(
                    x * CELL_SIZE + (CELL_SIZE - piece.getLocalBounds().width) / 2,
                    y * CELL_SIZE + (CELL_SIZE - piece.getLocalBounds().height) / 2
                );
                window.draw(piece);
            }
        }
    }
}

void handleMouseClick(const sf::Vector2i& mousePos) {
    int x = mousePos.x / CELL_SIZE;
    int y = mousePos.y / CELL_SIZE;

    if (x < 0 || x >= 10 || y < 0 || y >= 10) return;

    if (board[x][y] && board[x][y]->getColor() == PieceColor::White) {
        selectedPos = sf::Vector2i(x, y);
        possibleMoves = board[x][y]->getPossibleMoves(selectedPos, board);
    }
    else if (selectedPos.x != -1 &&
        std::find(possibleMoves.begin(), possibleMoves.end(), sf::Vector2i(x, y)) != possibleMoves.end()) {
        board[x][y] = board[selectedPos.x][selectedPos.y];
        board[selectedPos.x][selectedPos.y] = nullptr;
        board[x][y]->setMoved(true);
        selectedPos = sf::Vector2i(-1, -1);
        possibleMoves.clear();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess 10x10");
    loadTextures();
    initBoard();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            }
        }

        window.clear();
        drawBoard(window);

        // Подсветка выбранной клетки
        if (selectedPos.x != -1) {
            sf::RectangleShape highlight(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            highlight.setPosition(selectedPos.x * CELL_SIZE, selectedPos.y * CELL_SIZE);
            highlight.setFillColor(sf::Color(0, 255, 0, 100)); // Зелёный с прозрачностью
            window.draw(highlight);
        }

        // Подсветка возможных ходов
        for (const auto& move : possibleMoves) {
            sf::CircleShape marker(10.0f);  // Явно float
            marker.setPosition(
                static_cast<float>(move.x * CELL_SIZE) + CELL_SIZE / 2.0f - 10.0f,
                static_cast<float>(move.y * CELL_SIZE) + CELL_SIZE / 2.0f - 10.0f
            );
            marker.setFillColor(sf::Color(255, 255, 0, 150));
            window.draw(marker);
        }

        drawPieces(window);
        window.display();
    }

    // Очистка памяти
    for (auto& row : board) {
        for (auto& piece : row) {
            delete piece;
        }
    }

    return 0;
}