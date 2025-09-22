#ifndef SUPERSHOT_H
#define SUPERSHOT_H

#include "Spaceship.h"
#include "Bullet.h"
#include "GameConstants.h"

class SuperShot {
private:
    static constexpr int COST = 500; // Pontos necessários por supertiro
    int availableShots;
    sf::Clock cooldownClock;
    static constexpr float COOLDOWN = 2.0f; // Segundos entre supertiros
    
public:
    SuperShot();
    
    static int getCost() { return COST; } // Método estático
    
    bool canFire() const;
    bool hasShotsAvailable() const;
    void addShot();
    void addShotsBasedOnScore(int score);
    int getAvailableShots() const;
    void fire(Spaceship& player, std::vector<Bullet>& bullets, int& playerScore);
    void reset();
};

#endif