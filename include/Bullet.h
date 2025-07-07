#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"

class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool isActive;
    
    Bullet();
    void fire(sf::Vector2f pos, float angle, const sf::Color& color);
    void update(float deltaTime);
    bool isOutOfBounds() const;
    
private:
    static constexpr float SPEED = 800.0f;
    static constexpr float RADIUS = 4.0f;
};