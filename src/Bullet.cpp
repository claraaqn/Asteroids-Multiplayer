#include "Bullet.h"
#include <cmath>
#include "GameConstants.h"

using namespace GameConstants;

Bullet::Bullet() : isActive(false) {
    shape.setRadius(RADIUS);
    shape.setOrigin(RADIUS, RADIUS);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(200, 200, 200));
}

void Bullet::fire(sf::Vector2f pos, float angle, const sf::Color& color) {
    shape.setPosition(pos);
    shape.setFillColor(color);
    
    // Convert angle to radians (-90 para ajustar a orientação)
    float rad = (angle - 90) * (3.14159265f / 180.0f); // Usando PI diretamente
    velocity.x = std::cos(rad) * SPEED;
    velocity.y = std::sin(rad) * SPEED;
    
    isActive = true;
}

void Bullet::update(float deltaTime) {
    if (!isActive) return;
    
    shape.move(velocity * deltaTime);
    
    if (isOutOfBounds()) {
        isActive = false;
    }
}

bool Bullet::isOutOfBounds() const {
    const sf::Vector2f& pos = shape.getPosition();
    return pos.x < -50 || pos.x > WIDTH + 50 || pos.y < -50 || pos.y > HEIGHT + 50;
}