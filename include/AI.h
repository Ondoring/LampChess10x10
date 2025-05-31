#pragma once
#include <vector>
#include <algorithm>
#include "Piece.h"
#include "GameLogic.h"

class ImprovedAI {
public:
    static std::pair<sf::Vector2i, sf::Vector2i> findSafeMove(
        std::vector<std::vector<Piece*>>& board,
        PieceColor aiColor,
        const sf::Vector2i& currentKingPos,
        bool isInCheck)
    {
        std::vector<std::pair<sf::Vector2i, sf::Vector2i>> allPossibleMoves;

        // 1. Собрать все возможные ходы
        for (int x = 0; x < 10; ++x) {
            for (int y = 0; y < 10; ++y) {
                if (board[x][y] && board[x][y]->getColor() == aiColor) {
                    auto moves = board[x][y]->getPossibleMoves(sf::Vector2i(x, y), board);
                    for (const auto& move : moves) {
                        allPossibleMoves.emplace_back(sf::Vector2i(x, y), move);
                    }
                }
            }
        }

        // 2. Проверить каждый ход на безопасность
        std::vector<std::pair<sf::Vector2i, sf::Vector2i>> safeMoves;
        for (const auto& [from, to] : allPossibleMoves) {
            if (isMoveSafe(board, from, to, currentKingPos, aiColor, isInCheck)) {
                safeMoves.emplace_back(from, to);
            }
        }

        // 3. Если нет безопасных ходов - сдаёмся
        if (safeMoves.empty()) {
            return { {-1, -1}, {-1, -1} };
        }

        // 4. Выбрать случайный безопасный ход
        return safeMoves[rand() % safeMoves.size()];
    }

private:
    static bool isMoveSafe(
        std::vector<std::vector<Piece*>>& board,
        const sf::Vector2i& from,
        const sf::Vector2i& to,
        const sf::Vector2i& kingPos,
        PieceColor aiColor,
        bool isInCheck)
    {
        // Сохраняем состояние доски
        Piece* movingPiece = board[from.x][from.y];
        Piece* capturedPiece = board[to.x][to.y];

        // Делаем временный ход
        board[to.x][to.y] = movingPiece;
        board[from.x][from.y] = nullptr;

        // Определяем новую позицию короля (если двигали короля)
        sf::Vector2i newKingPos = (movingPiece->getType() == PieceType::King) ? to : kingPos;

        // Проверяем, остался ли король под атакой
        bool kingSafe = !isKingUnderAttack(newKingPos, board);

        // Отменяем временный ход
        board[from.x][from.y] = movingPiece;
        board[to.x][to.y] = capturedPiece;

        // Особые условия при шахе
        if (isInCheck) {
            // Ход должен обязательно устранять шах
            return kingSafe;
        }

        return kingSafe;
    }
};