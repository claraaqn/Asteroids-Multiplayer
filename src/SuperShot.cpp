#include "SuperShot.h"
#include <cmath>

using namespace GameConstants;

SuperShot::SuperShot() : availableShots(0) {}

bool SuperShot::canFire() const {
    return cooldownClock.getElapsedTime().asSeconds() > COOLDOWN;
}

bool SuperShot::hasShotsAvailable() const {
    return availableShots > 0;
}

void SuperShot::addShot() {
    availableShots++;
}

void SuperShot::addShotsBasedOnScore(int score) {
    int expectedShots = score / COST;
    if (expectedShots > availableShots) {
        availableShots = expectedShots;
    }
}

int SuperShot::getAvailableShots() const {
    return availableShots;
}


void SuperShot::fire(Spaceship& player, std::vector<Bullet>& bullets, int& playerScore) {
    if (!canFire() || !hasShotsAvailable() || playerScore < COST) {
        return;
    }
    
    // Gasta os pontos e um supertiro
    playerScore -= COST;
    availableShots--;
    cooldownClock.restart();
    
    // Ângulos para os 8 tiros (frente, trás, lados e diagonais)
    float playerAngle = player.angle;
    std::vector<float> angles = {
        playerAngle,           // Frente
        playerAngle + 180,     // Trás
        playerAngle + 90,      // Direita
        playerAngle - 90,      // Esquerda
        playerAngle + 45,      // Diagonal frente-direita
        playerAngle - 45,      // Diagonal frente-esquerda
        playerAngle + 135,     // Diagonal trás-direita
        playerAngle - 135      // Diagonal trás-esquerda
    };
    
    // Posições de disparo relativas à nave
    sf::Vector2f center = player.sprite.getPosition();
    std::vector<sf::Vector2f> positions = {
        center + sf::Vector2f(0, -25),  // Frente
        center + sf::Vector2f(0, 25),   // Trás
        center + sf::Vector2f(25, 0),   // Direita
        center + sf::Vector2f(-25, 0),  // Esquerda
        center + sf::Vector2f(18, -18), // Diagonal frente-direita
        center + sf::Vector2f(-18, -18),// Diagonal frente-esquerda
        center + sf::Vector2f(18, 18),  // Diagonal trás-direita
        center + sf::Vector2f(-18, 18)  // Diagonal trás-esquerda
    };
    
    // Dispara todos os tiros
    for (size_t i = 0; i < angles.size(); i++) {
        bullets.emplace_back().fire(positions[i], angles[i]);
    }
}

void SuperShot::reset() {
    availableShots = 0;
    cooldownClock.restart();
}