#pragma once
#include "Piece.h"
#include <vector>

class Bishop : public Piece {
public:
    Bishop(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Bishop; }

    std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const override {
        std::vector<sf::Vector2i> moves;
        const int directions[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} }; // Диагонали

        for (const auto& dir : directions) {
            for (int i = 1; i < 10; ++i) {
                sf::Vector2i newPos(position.x + dir[0] * i, position.y + dir[1] * i);

                if (newPos.x < 0 || newPos.x >= 10 || newPos.y < 0 || newPos.y >= 10)
                    break;

                if (!board[newPos.x][newPos.y]) {
                    moves.push_back(newPos);
                }
                else {
                    if (board[newPos.x][newPos.y]->getColor() != color)
                        moves.push_back(newPos);
                    break;
                }
            }
        }
        return moves;
    }
};