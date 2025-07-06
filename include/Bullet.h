#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"

class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool isActive;
    static constexpr float SPEED = 1200.0f;  // Velocidade constante alta

    Bullet();
    void fire(sf::Vector2f pos, float angle);
    void update(float deltaTime);  // Adicionado deltaTime
    bool isOutOfBounds() const;
};