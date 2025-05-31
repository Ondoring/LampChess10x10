#pragma once
#include "Piece.h"
#include <vector>

bool isKingUnderAttack(const sf::Vector2i& kingPos, const std::vector<std::vector<Piece*>>& board);
