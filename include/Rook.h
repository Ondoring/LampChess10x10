#pragma once
#include "Piece.h"
#include <vector>

class Rook : public Piece {
public:
    Rook(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Rook; }

    std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const override {
        std::vector<sf::Vector2i> moves;

        // Горизонталь и вертикаль
        const int directions[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

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