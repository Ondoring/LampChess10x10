#pragma once
#include "Piece.h"
#include "Rook.h" // Для dynamic_cast
#include <vector>

class King : public Piece {
public:
    King(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::King; }

    std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const override {
        std::vector<sf::Vector2i> moves;
        const int moveset[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1},
                                  {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

        for (const auto& m : moveset) {
            sf::Vector2i newPos(position.x + m[0], position.y + m[1]);

            if (newPos.x >= 0 && newPos.x < 10 && newPos.y >= 0 && newPos.y < 10) {
                if (!board[newPos.x][newPos.y] || board[newPos.x][newPos.y]->getColor() != color)
                    moves.push_back(newPos);
            }
        }

        // Рокировка
        if (!hasMoved()) {
            // Короткая (вправо)
            if (position.x + 2 < 10 && board[9][position.y] &&
                dynamic_cast<Rook*>(board[9][position.y]) &&
                !board[9][position.y]->hasMoved()) {
                bool pathClear = true;
                for (int x = position.x + 1; x < 9; ++x) {
                    if (board[x][position.y]) pathClear = false;
                }
                if (pathClear) moves.push_back({ position.x + 2, position.y });
            }
            // Длинная (влево) - аналогично
        }

        return moves;
    }
};