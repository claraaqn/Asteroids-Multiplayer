#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"
#include "Spaceship.h"
#include "GameConstants.h"

class SuperShot {
private:
    static constexpr int COST = 500; // Pontos necessários por supertiro
    static constexpr float COOLDOWN = 0.5f; // Cooldown em segundos
    int availableShots;
    sf::Clock cooldownClock;
    
public:
    SuperShot();
    
    bool canFire() const;
    bool hasShotsAvailable() const;
    void addShot();
    void addShotsBasedOnScore(int score);
    int getAvailableShots() const;
    int getCost() const;
    
    void fire(Spaceship& player, std::vector<Bullet>& bullets, int& playerScore);
    void reset();
};