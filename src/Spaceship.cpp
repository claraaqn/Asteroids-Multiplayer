#include "Spaceship.h" // Inclui a DECLARAÇÃO da classe Spaceship
#include <cmath> // Para std::cos, std::sin

using namespace GameConstants; // Para usar PI, WIDTH, HEIGHT diretamente

// Definição do construtor
Spaceship::Spaceship(sf::Vector2f startPos, float startAngle, bool player1) {
    position = startPos;
    angle = startAngle;
    velocity = sf::Vector2f(0, 0);
    isAlive = true;
    isPlayer1 = player1;

    shape.setPointCount(7);

    shape.setPoint(0, sf::Vector2f(15, 0));    // Ponta da nave
    shape.setPoint(1, sf::Vector2f(-10, -10)); // Asa superior traseira
    shape.setPoint(2, sf::Vector2f(-5, -5));   // Curva superior
    shape.setPoint(3, sf::Vector2f(-10, 0));   // Centro traseiro
    shape.setPoint(4, sf::Vector2f(-5, 5));    // Curva inferior
    shape.setPoint(5, sf::Vector2f(-10, 10));  // Asa inferior traseira
    shape.setPoint(6, sf::Vector2f(15, 0));    // Fecha o polígono

    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(2);
    shape.setPosition(position);
    shape.setRotation(angle);

    float sumX = 0;
    float sumY = 0;
    int count = shape.getPointCount();

    for (int i = 0; i < count; i++) {
        sf::Vector2f p = shape.getPoint(i);
        sumX += p.x;
        sumY += p.y;
    }

    float centerX = sumX / count;
    float centerY = sumY / count;

    shape.setOrigin(centerX, centerY);
}

void Spaceship::update() {
    if (!isAlive) return;

    position += velocity;

    if (isPlayer1) {
        if (position.x < 0) position.x = 0;
        if (position.x > WIDTH/2) position.x = WIDTH/2;
    } else {
        if (position.x < WIDTH/2) position.x = WIDTH/2;
        if (position.x > WIDTH) position.x = WIDTH;
    }

    if (position.y < 0) position.y = 0;
    if (position.y > HEIGHT) position.y = HEIGHT;

    shape.setPosition(position);
    shape.setRotation(angle);
}

void Spaceship::accelerate(float amount) {
    float rad = (angle - 90) * PI / 180;
    sf::Vector2f acceleration(
        amount * std::cos(rad),
        amount * std::sin(rad)
    );

    velocity += acceleration;

    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > 5.0f) { 
        velocity = (velocity / speed) * 5.0f;
    }
}

void Spaceship::decelerate() {
    velocity *= 0.98f; 

    if (std::abs(velocity.x) < 0.01f && std::abs(velocity.y) < 0.01f) {
        velocity = sf::Vector2f(0, 0);
    }
}

sf::Vector2f Spaceship::getFirePosition() const { 
    float rad = (angle - 90) * PI / 180;
    return sf::Vector2f(
        position.x + 25 * std::cos(rad),
        position.y + 25 * std::sin(rad)
    );
}

bool Spaceship::canFire() const { 
    return fireCooldown.getElapsedTime().asMilliseconds() > 300;
}

void Spaceship::resetFireCooldown() {
    fireCooldown.restart();
}