#pragma once
#include "Piece.h"
#include "GameLogic.h"

class King : public Piece {
public:
    King(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::King; }

    std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const override
    {
        std::vector<sf::Vector2i> moves;

        // —тандартные ходы корол€
        const int kingMoves[8][2] = { {1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1} };
        for (const auto& move : kingMoves) {
            sf::Vector2i newPos(position.x + move[0], position.y + move[1]);
            if (newPos.x >= 0 && newPos.x < 10 && newPos.y >= 0 && newPos.y < 10) {
                if (!board[newPos.x][newPos.y] || board[newPos.x][newPos.y]->getColor() != color) {
                    moves.push_back(newPos);
                }
            }
        }

        // –окировка
        if (!hasMoved()) {
            // ѕроверка короткой рокировки (вправо)
            if (position.x + 3 < 10) {
                Piece* rook = board[position.x + 3][position.y];
                if (rook && rook->getType() == PieceType::Rook && !rook->hasMoved()) {
                    bool pathClear = true;
                    for (int x = position.x + 1; x < position.x + 3; ++x) {
                        if (board[x][position.y]) {
                            pathClear = false;
                            break;
                        }
                        // ѕроверка, не проходит ли король через атакованное поле
                        if (isKingUnderAttack(sf::Vector2i(x, position.y), board)) {
                            pathClear = false;
                            break;
                        }
                    }
                    if (pathClear) moves.push_back({ position.x + 2, position.y });
                }
            }

            // ѕроверка длинной рокировки (влево)
            if (position.x - 4 >= 0) {
                Piece* rook = board[position.x - 4][position.y];
                if (rook && rook->getType() == PieceType::Rook && !rook->hasMoved()) {
                    bool pathClear = true;
                    for (int x = position.x - 1; x > position.x - 4; --x) {
                        if (board[x][position.y]) {
                            pathClear = false;
                            break;
                        }
                        // ѕроверка, не проходит ли король через атакованное поле
                        if (isKingUnderAttack(sf::Vector2i(x, position.y), board)) {
                            pathClear = false;
                            break;
                        }
                    }
                    if (pathClear) moves.push_back({ position.x - 2, position.y });
                }
            }
        }
        return moves;
    }
};