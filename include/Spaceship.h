#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"

class Spaceship {
public:
    sf::ConvexShape shape;
    sf::Vector2f position;
    float angle;
    sf::Vector2f velocity;
    bool isAlive;
    bool isPlayer1;
    float fireCooldownMax;

    Spaceship(sf::Vector2f startPos, float startAngle, bool player1);
    void update();
    void accelerate(float amount);
    void decelerate();
    sf::Vector2f getFirePosition() const;
    bool canFire() const;
    void resetFireCooldown();

private:
    float FIRE_COOLDOWN = 0.3f; 
    sf::Clock fireCooldown;
};