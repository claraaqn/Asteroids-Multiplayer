#include "Bullet.h"
#include <cmath>
#include "GameConstants.h"

using namespace GameConstants;

Bullet::Bullet() {
    shape.setRadius(5);  // Aumentei o raio para melhor visibilidade
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(5, 5);  // Origem centralizada
    isActive = false;
}

void Bullet::fire(sf::Vector2f pos, float angle) {
    shape.setPosition(pos);
    float rad = (angle - 90) * PI / 180;
    
    velocity.x = std::cos(rad) * SPEED;
    velocity.y = std::sin(rad) * SPEED;
    isActive = true;
}

void Bullet::update(float deltaTime) {
    if (!isActive) return;
    
    shape.move(velocity * deltaTime);
    
    sf::Vector2f pos = shape.getPosition();
    if (pos.x < 0 || pos.x > WIDTH || pos.y < 0 || pos.y > HEIGHT) {
        isActive = false;
    }
}