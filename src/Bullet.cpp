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

    sf::Vector2f previousPosition = shape.getPosition();
    shape.move(velocity);
    sf::Vector2f currentPosition = shape.getPosition();

    // Verifica se saiu da tela
    if (currentPosition.x < 0 || currentPosition.x > WIDTH ||
        currentPosition.y < 0 || currentPosition.y > HEIGHT) {
        isActive = false;
        return;
    }

    // Verifica se cruzou a linha divisória (só verifica se houve mudança de lado)
    if ((previousPosition.x < WIDTH/2 && currentPosition.x >= WIDTH/2) || 
        (previousPosition.x >= WIDTH/2 && currentPosition.x < WIDTH/2)) {
        isActive = false;
    }
}

