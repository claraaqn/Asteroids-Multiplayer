#include "Bullet.h" // Inclui a declaração da classe Bullet
#include <cmath> // Para std::cos, std::sin
#include "GameConstants.h" // Para PI, WIDTH, HEIGHT

using namespace GameConstants;

Bullet::Bullet() {
    shape.setRadius(3);
    shape.setFillColor(sf::Color::White);
    isActive = false;
    shape.setOrigin(0, 0); // Substituir por uma origem melhor

}

void Bullet::fire(sf::Vector2f pos, float angle) {
    float rad = (angle - 90) * PI / 180;
    shape.setPosition(pos);
    velocity.x = 10 * std::cos(rad);
    velocity.y = 10 * std::sin(rad);
    isActive = true;
}

void Bullet::update() {
    if (!isActive) return;

    shape.move(velocity);

    // Verifica se saiu da tela
    if (shape.getPosition().x < 0 || shape.getPosition().x > WIDTH ||
        shape.getPosition().y < 0 || shape.getPosition().y > HEIGHT) {
        isActive = false;
    }
}
