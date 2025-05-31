#include <SFML/Graphics.hpp>
#include "include/TextureHolder.h"
#include "include/Piece.h"
#include "include/Pawn.h"
#include "include/Bishop.h"
#include "include/King.h"
#include "include/Knight.h"
#include "include/Queen.h"
#include "include/Rook.h"
#include "include/GameLogic.h"
#include "include/AI.h"
#include "include/Menu.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdlib> // Для rand()
#include <ctime>   // Для time()

const int CELL_SIZE = 80; // Размер клетки в пикселях
const sf::Color LIGHT_COLOR(240, 217, 181);
const sf::Color DARK_COLOR(181, 136, 99);
const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 1000;
const int BOARD_OFFSET_X = 40;
const int BOARD_OFFSET_Y = 40;

TextureHolder textures;
std::vector<std::vector<Piece*>> board(10, std::vector<Piece*>(10, nullptr));
sf::Vector2i selectedPos(-1, -1);
std::vector<sf::Vector2i> possibleMoves;
bool isWhiteTurn = true;
bool isCheck = false;
sf::Vector2i kingPosWhite(5, 8);
sf::Vector2i kingPosBlack(5, 1);

enum class GameState {
    InMenu,
    InGame
};

void filterLegalMoves();
void movePiece(const sf::Vector2i& from, const sf::Vector2i& to);
void updateKingPosition();
bool isCheckmate(bool isWhite);

void resetGameState() {
    selectedPos = sf::Vector2i(-1, -1);
    possibleMoves.clear();
    isWhiteTurn = true;
    isCheck = false;
    kingPosWhite = sf::Vector2i(5, 8);
    kingPosBlack = sf::Vector2i(5, 1);
}

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

    for (int x = 1; x < 9; ++x) {
        board[x][7] = new Pawn(PieceColor::White);
        board[x][7]->setSprite(textures.get("white_pawn"));

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
    // Рамка вокруг доски
    sf::RectangleShape border(sf::Vector2f(CELL_SIZE * 10 + 10, CELL_SIZE * 10 + 10));
    border.setPosition(BOARD_OFFSET_X - 5, BOARD_OFFSET_Y - 5);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(10);
    border.setOutlineColor(sf::Color(139, 69, 19));
    window.draw(border);

    // Сама доска
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(BOARD_OFFSET_X + x * CELL_SIZE, BOARD_OFFSET_Y + y * CELL_SIZE);
            cell.setFillColor((x + y) % 2 == 0 ? LIGHT_COLOR : DARK_COLOR);
            window.draw(cell);
        }
    }
}

void drawPieces(sf::RenderWindow& window) {
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (board[x][y] != nullptr) {
                sf::Sprite piece = board[x][y]->getSprite();
                float offsetX = (static_cast<float>(CELL_SIZE) - piece.getLocalBounds().width) / 2.0f;
                float offsetY = (static_cast<float>(CELL_SIZE) - piece.getLocalBounds().height) / 2.0f;
                piece.setPosition(
                    static_cast<float>(x * CELL_SIZE) + offsetX + BOARD_OFFSET_X,
                    static_cast<float>(y * CELL_SIZE) + offsetY + BOARD_OFFSET_Y
                );
                window.draw(piece);
            }
        }
    }
}

void handleMouseClick(const sf::Vector2i& mousePos) {
    int x = (mousePos.x - BOARD_OFFSET_X + 40) / CELL_SIZE;
    int y = (mousePos.y - BOARD_OFFSET_Y + 40) / CELL_SIZE;

    if (x < 0 || x >= 10 || y < 0 || y >= 10) return;

    // Выбор фигуры
    if (board[x][y] && board[x][y]->getColor() == (isWhiteTurn ? PieceColor::White : PieceColor::Black)) {
        selectedPos = sf::Vector2i(x, y);
        possibleMoves = board[x][y]->getPossibleMoves(selectedPos, board);
        filterLegalMoves();
        return;
    }

    // Выполнение хода
    if (selectedPos.x != -1 && std::find(possibleMoves.begin(), possibleMoves.end(), sf::Vector2i(x, y)) != possibleMoves.end()) {
        movePiece(selectedPos, sf::Vector2i(x, y));
        isWhiteTurn = !isWhiteTurn;
        updateKingPosition();
        isCheck = isKingUnderAttack(isWhiteTurn ? kingPosWhite : kingPosBlack, board);
        selectedPos = sf::Vector2i(-1, -1);
        possibleMoves.clear();
    }
}

void movePiece(const sf::Vector2i& from, const sf::Vector2i& to) {
    // Проверка валидности позиций
    if (from.x < 0 || from.x >= 10 || from.y < 0 || from.y >= 10 ||
        to.x < 0 || to.x >= 10 || to.y < 0 || to.y >= 10 ||
        board[from.x][from.y] == nullptr) return;

    Piece* movingPiece = board[from.x][from.y];

    // Проверка на превращение пешки
    if (movingPiece->getType() == PieceType::Pawn && (to.y == 0 || to.y == 9)) {
        PieceColor color = movingPiece->getColor();
        delete board[from.x][from.y]; // Удаляем пешку

        // Создаём ферзя вместо пешки
        board[from.x][from.y] = new Queen(color);
        std::string textureName = (color == PieceColor::White) ? "white_queen" : "black_queen";
        board[from.x][from.y]->setSprite(textures.get(textureName));
    }

    // Обработка рокировки
    King* king = dynamic_cast<King*>(board[from.x][from.y]);
    if (king != nullptr && abs(from.x - to.x) == 2) {
        // Короткая рокировка
        if (to.x > from.x && to.x + 1 < 10 && board[to.x + 1][to.y] != nullptr) {
            board[to.x - 1][to.y] = board[to.x + 1][to.y];
            board[to.x + 1][to.y] = nullptr;
            board[to.x - 1][to.y]->setMoved(true);
        }
        // Длинная рокировка
        else if (to.x < from.x && to.x - 2 >= 0 && board[to.x - 2][to.y] != nullptr) {
            board[to.x + 1][to.y] = board[to.x - 2][to.y];
            board[to.x - 2][to.y] = nullptr;
            board[to.x + 1][to.y]->setMoved(true);
        }
    }

    // Стандартное перемещение
    delete board[to.x][to.y];
    board[to.x][to.y] = board[from.x][from.y];
    board[from.x][from.y] = nullptr;
    board[to.x][to.y]->setMoved(true);

    // Обновляем позицию короля
    if (king != nullptr) {
        if (king->getColor() == PieceColor::White) {
            kingPosWhite = to;
        }
        else {
            kingPosBlack = to;
        }
    }
}

bool isCheckmate(bool forWhite) {
    const sf::Vector2i& kingPos = forWhite ? kingPosWhite : kingPosBlack;
    PieceColor color = forWhite ? PieceColor::White : PieceColor::Black;

    // Проверяем все возможные ходы
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (board[x][y] && board[x][y]->getColor() == color) {
                auto moves = board[x][y]->getPossibleMoves(sf::Vector2i(x, y), board);
                for (const auto& move : moves) {
                    // Пробуем временный ход
                    Piece* temp = board[move.x][move.y];
                    board[move.x][move.y] = board[x][y];
                    board[x][y] = nullptr;

                    // Обновляем позицию короля, если двигаем короля
                    sf::Vector2i tempKingPos = kingPos;
                    if (board[move.x][move.y]->getType() == PieceType::King) {
                        tempKingPos = move;
                    }

                    bool stillInCheck = isKingUnderAttack(tempKingPos, board);

                    // Отменяем ход
                    board[x][y] = board[move.x][move.y];
                    board[move.x][move.y] = temp;

                    if (!stillInCheck) {
                        return false; // Нашли ход, снимающий шах
                    }
                }
            }
        }
    }
    return true; // Нет ходов, снимающих шах
}

void filterLegalMoves() {
    std::vector<sf::Vector2i> legalMoves;

    if (selectedPos.x < 0 || selectedPos.x >= 10 ||
        selectedPos.y < 0 || selectedPos.y >= 10 ||
        !board[selectedPos.x][selectedPos.y]) {
        possibleMoves.clear();
        return;
    }

    Piece* movingPiece = board[selectedPos.x][selectedPos.y];
    PieceColor movingColor = movingPiece->getColor();
    sf::Vector2i originalKingPos = (movingColor == PieceColor::White) ? kingPosWhite : kingPosBlack;

    for (const auto& move : possibleMoves) {
        // Временное сохранение состояния
        Piece* temp = board[move.x][move.y];
        board[move.x][move.y] = movingPiece;
        board[selectedPos.x][selectedPos.y] = nullptr;

        // Обновляем позицию короля, если двигаем короля
        sf::Vector2i newKingPos = originalKingPos;
        if (movingPiece->getType() == PieceType::King) {
            newKingPos = move;
        }

        bool kingSafe = !isKingUnderAttack(newKingPos, board);

        // Восстанавливаем состояние
        board[selectedPos.x][selectedPos.y] = movingPiece;
        board[move.x][move.y] = temp;

        if (kingSafe) {
            legalMoves.push_back(move);
        }
    }

    possibleMoves = legalMoves;
}

void updateKingPosition() {
    kingPosWhite = sf::Vector2i(-1, -1);
    kingPosBlack = sf::Vector2i(-1, -1);

    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (board[x][y] && board[x][y]->getType() == PieceType::King) {
                if (board[x][y]->getColor() == PieceColor::White) {
                    kingPosWhite = sf::Vector2i(x, y);
                }
                else {
                    kingPosBlack = sf::Vector2i(x, y);
                }
            }
        }
    }
}

void drawGameInfo(sf::RenderWindow& window, const sf::Font& font, bool isWhiteTurn, bool isCheck) {
    sf::RectangleShape panel(sf::Vector2f(300, 400));
    panel.setPosition(950, 100);
    panel.setFillColor(sf::Color(240, 240, 240));
    window.draw(panel);

    sf::Text turnText;
    turnText.setFont(font);
    turnText.setString("Current turn: " + std::string(isWhiteTurn ? "White" : "Black"));
    turnText.setPosition(975, 120);
    turnText.setFillColor(sf::Color::Black);
    window.draw(turnText);

    if (isCheck) {
        if (!isCheckmate(isWhiteTurn)) {
            sf::Text checkText;
            checkText.setFont(font);
            checkText.setString("CHECK!");
            checkText.setPosition(1000, 170);
            checkText.setFillColor(sf::Color::Red);
            window.draw(checkText);
        }
    }


    // Кнопка возврата в меню
    sf::RectangleShape menuButton(sf::Vector2f(200, 50));
    menuButton.setPosition(1000, 600);
    menuButton.setFillColor(sf::Color(100, 100, 100));
    window.draw(menuButton);

    sf::Text menuText;
    menuText.setFont(font);
    menuText.setString("Main Menu");
    menuText.setPosition(1000 + (200 - menuText.getLocalBounds().width) / 2, 610);
    menuText.setFillColor(sf::Color::White);
    window.draw(menuText);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lamp Chess 10x10");
    loadTextures();
    initBoard();
    std::srand(std::time(nullptr));
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        return -1;
    }

    Menu mainMenu(window);
    GameState gameState = GameState::InMenu;
    bool vsAI = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (gameState == GameState::InMenu) {
                    int choice = mainMenu.handleClick(mousePos);
                    if (choice == 1) { // Play vs Friend
                        gameState = GameState::InGame;
                        vsAI = false;
                        initBoard();
                        resetGameState();
                        
                    }
                    else if (choice == 2) { // Play vs AI
                        gameState = GameState::InGame;
                        vsAI = true;
                        initBoard();
                        resetGameState();
                    }
                    else if (choice == 3) { // Exit
                        window.close();
                    }
                }
                else if (gameState == GameState::InGame) {
                    // Обработка кликов в игре
                    if (mousePos.x >= 1000 && mousePos.x <= 1200 &&
                        mousePos.y >= 600 && mousePos.y <= 650) {
                        gameState = GameState::InMenu; // Возврат в меню
                    }
                    else {
                        handleMouseClick(mousePos - sf::Vector2i(BOARD_OFFSET_X, BOARD_OFFSET_Y));

                        // Ход ИИ
                        if (vsAI && !isWhiteTurn) {
                            bool aiInCheck = isKingUnderAttack(kingPosBlack, board);
                            auto aiMove = ImprovedAI::findSafeMove(board, PieceColor::Black, kingPosBlack, aiInCheck);

                            if (aiMove.first.x != -1) {
                                movePiece(aiMove.first, aiMove.second);
                                isWhiteTurn = true;
                                updateKingPosition();
                                isCheck = isKingUnderAttack(kingPosBlack, board);

                                // Проверка на мат после хода ИИ
                                if (isCheck && isCheckmate(true)) {
                                    sf::Text checkmateText;
                                    checkmateText.setFont(font);
                                    checkmateText.setString("CHECKMATE!");
                                    checkmateText.setPosition(1000, 170);
                                    checkmateText.setFillColor(sf::Color::Red);
                                    window.draw(checkmateText);
                                }
                            }
                            else {
                                // Если нет допустимых ходов
                                if (aiInCheck) {
                                    sf::Text checkmateText;
                                    checkmateText.setFont(font);
                                    checkmateText.setString("CHECKMATE!");
                                    checkmateText.setPosition(1000, 170);
                                    checkmateText.setFillColor(sf::Color::Red);
                                    window.draw(checkmateText);
                                }
                                else {
                                    sf::Text checkmateText;
                                    checkmateText.setFont(font);
                                    checkmateText.setString("STALEMATE!");
                                    checkmateText.setPosition(1000, 170);
                                    checkmateText.setFillColor(sf::Color::Red);
                                    window.draw(checkmateText);
                                }
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(50, 50, 50));

        if (gameState == GameState::InMenu) {
            mainMenu.draw();
        }
        else if (gameState == GameState::InGame) {
            drawBoard(window);

            // Подсветка выбранной клетки
            if (selectedPos.x != -1) {
                sf::RectangleShape highlight(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                highlight.setPosition(selectedPos.x * CELL_SIZE + BOARD_OFFSET_X, selectedPos.y * CELL_SIZE + BOARD_OFFSET_Y);
                highlight.setFillColor(sf::Color(0, 255, 0, 100));
                window.draw(highlight);
            }

            // Королю шах
            if (isCheck) {
                sf::Vector2i kingPos = isWhiteTurn ? kingPosWhite : kingPosBlack;
                sf::RectangleShape highlight(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                highlight.setPosition(kingPos.x * CELL_SIZE + BOARD_OFFSET_X, kingPos.y * CELL_SIZE + BOARD_OFFSET_Y);
                highlight.setFillColor(sf::Color(255, 0, 0, 100));
                window.draw(highlight);
            }

            drawPieces(window);

            // Подсветка возможных ходов
            for (const auto& move : possibleMoves) {
                sf::CircleShape marker(10.0f);
                marker.setPosition(
                    static_cast<float>(move.x * CELL_SIZE) + BOARD_OFFSET_X + CELL_SIZE / 2.0f - 10.0f,
                    static_cast<float>(move.y * CELL_SIZE) + BOARD_OFFSET_Y + CELL_SIZE / 2.0f - 10.0f
                );
                marker.setFillColor(sf::Color(255, 255, 0, 150));
                window.draw(marker);
            }
            if (selectedPos.x >= 0 && selectedPos.x < 10 &&
                selectedPos.y >= 0 && selectedPos.y < 10 &&
                board[selectedPos.x][selectedPos.y] != nullptr)
            {
                King* king = dynamic_cast<King*>(board[selectedPos.x][selectedPos.y]);
                if (king != nullptr) {
                    for (const auto& move : possibleMoves) {
                        if (abs(move.x - selectedPos.x) == 2) {
                            sf::RectangleShape rect(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                            rect.setPosition(move.x * CELL_SIZE + BOARD_OFFSET_X, move.y * CELL_SIZE + BOARD_OFFSET_Y);
                            rect.setFillColor(sf::Color(100, 200, 255, 150));
                            window.draw(rect);
                        }
                    }
                }
            }

            drawGameInfo(window, font, isWhiteTurn, isCheck);
        }

        window.display();
    }

    return 0;
}