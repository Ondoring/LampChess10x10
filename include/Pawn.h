#pragma once
#include "Piece.h"
#include <vector>

class Pawn : public Piece {
public:
    Pawn(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Pawn; }

    std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const override
    {
        std::vector<sf::Vector2i> moves;
        int direction = (color == PieceColor::White) ? -1 : 1;

        // Ход вперёд
        sf::Vector2i forward(position.x, position.y + direction);
        if (forward.y >= 0 && forward.y < 10 && !board[forward.x][forward.y]) {
            moves.push_back(forward);

            // Первый ход на 2 клетки
            if (!hasMoved() && !board[position.x][position.y + 2 * direction]) {
                moves.push_back({ position.x, position.y + 2 * direction });
            }
        }

        // Взятие
        for (int dx : {-1, 1}) {
            sf::Vector2i capturePos(position.x + dx, position.y + direction);
            if (capturePos.x >= 0 && capturePos.x < 10 && capturePos.y >= 0 && capturePos.y < 10) {
                if (board[capturePos.x][capturePos.y] &&
                    board[capturePos.x][capturePos.y]->getColor() != color) {
                    moves.push_back(capturePos);
                }
            }
        }

        return moves;
    }
};