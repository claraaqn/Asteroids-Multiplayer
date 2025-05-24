#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"

class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool isActive;

    Bullet();
    void fire(sf::Vector2f pos, float angle);
    void update();
};