#include "GameLogic.h"
#include "King.h"

bool isKingUnderAttack(const sf::Vector2i& kingPos, const std::vector<std::vector<Piece*>>& board) {
    if (kingPos.x < 0 || kingPos.x >= 10 || kingPos.y < 0 || kingPos.y >= 10) {
        return false;
    }

    Piece* kingPiece = board[kingPos.x][kingPos.y];
    if (!kingPiece || kingPiece->getType() != PieceType::King) {
        return false;
    }

    PieceColor kingColor = kingPiece->getColor();

    // Проверяем все фигуры противника
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (board[x][y] && board[x][y]->getColor() != kingColor) {
                auto moves = board[x][y]->getPossibleMoves(sf::Vector2i(x, y), board);
                if (std::find(moves.begin(), moves.end(), kingPos) != moves.end()) {
                    return true;
                }
            }
        }
    }
    return false;
}