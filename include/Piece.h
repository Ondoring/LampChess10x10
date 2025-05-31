#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class PieceColor { White, Black };
enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King };

class Piece {
protected:
    PieceColor color;
    sf::Sprite sprite;
    bool hasMoved_ = false;  // Переименовано для ясности

public:
    Piece(PieceColor c) : color(c) {}
    virtual ~Piece() = default;

    virtual PieceType getType() const = 0;
    virtual std::vector<sf::Vector2i> getPossibleMoves(const sf::Vector2i& position,
        const std::vector<std::vector<Piece*>>& board) const = 0;

    PieceColor getColor() const { return color; }
    void setSprite(const sf::Texture& texture) { sprite.setTexture(texture); }
    const sf::Sprite& getSprite() const { return sprite; }
    void setMoved(bool moved) { hasMoved_ = moved; }
    bool hasMoved() const { return hasMoved_; }  // Геттер
};