#pragma once
#include "Piece.h"
#include <vector>

class Knight : public Piece {
public:
    Knight(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Knight; }

    std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const override {
        std::vector<sf::Vector2i> moves;
        const int moveset[8][2] = { {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                  {1, 2}, {1, -2}, {-1, 2}, {-1, -2} };

        for (const auto& m : moveset) {
            sf::Vector2i newPos(position.x + m[0], position.y + m[1]);

            if (newPos.x >= 0 && newPos.x < 10 && newPos.y >= 0 && newPos.y < 10) {
                if (!board[newPos.x][newPos.y] || board[newPos.x][newPos.y]->getColor() != color)
                    moves.push_back(newPos);
            }
        }
        return moves;
    }
};