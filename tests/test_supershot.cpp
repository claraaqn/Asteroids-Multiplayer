#include "catch_amalgamated.hpp"
#include "../include/SuperShot.h"
#include "../include/Spaceship.h"
#include "../include/Bullet.h"
#include "../include/GameConstants.h"

TEST_CASE("SuperShot Initialization", "[SuperShot]") {
    SuperShot superShot;
    
    SECTION("Starts with zero available shots") {
        REQUIRE(superShot.getAvailableShots() == 0);
    }
    
    SECTION("Cooldown allows firing initially") {
        REQUIRE(superShot.canFire()); // Deve poder disparar imediatamente
    }
}

TEST_CASE("SuperShot Management", "[SuperShot]") {
    SuperShot superShot;
    
    SECTION("Add shots increases available count") {
        superShot.addShot();
        REQUIRE(superShot.getAvailableShots() == 1);
        
        superShot.addShot();
        REQUIRE(superShot.getAvailableShots() == 2);
    }
    
    SECTION("Add shots based on score") {
        superShot.addShotsBasedOnScore(500); // Deve dar 1 shot (500/500)
        REQUIRE(superShot.getAvailableShots() == 1);
        
        superShot.addShotsBasedOnScore(1000); // Deve dar 2 shots
        REQUIRE(superShot.getAvailableShots() == 2);
    }
    
    SECTION("Reset clears available shots") {
        superShot.addShot();
        superShot.reset();
        REQUIRE(superShot.getAvailableShots() == 0);
    }
}

TEST_CASE("SuperShot Firing", "[SuperShot]") {
    SuperShot superShot;
    Spaceship ship(sf::Vector2f(400.0f, 300.0f), 0.0f, true);
    std::vector<Bullet> bullets;
    int playerScore = 1000;
    
    SECTION("Fire consumes shot and score") {
        superShot.addShot();
        int initialScore = playerScore;
        
        superShot.fire(ship, bullets, playerScore);
        
        REQUIRE(superShot.getAvailableShots() == 0);
        REQUIRE(playerScore == initialScore - SuperShot::getCost());
    }
    
    SECTION("Fire creates multiple bullets") {
        superShot.addShot();
        size_t initialBulletCount = bullets.size();
        
        superShot.fire(ship, bullets, playerScore);
        
        REQUIRE(bullets.size() == initialBulletCount + 8); // 8 tiros do super
    }
    
    SECTION("Cannot fire without shots") {
        int initialScore = playerScore;
        size_t initialBulletCount = bullets.size();
        
        superShot.fire(ship, bullets, playerScore); // Não deve fazer nada
        
        REQUIRE(playerScore == initialScore);
        REQUIRE(bullets.size() == initialBulletCount);
    }
    
    SECTION("Cannot fire without enough score") {
        superShot.addShot();
        int lowScore = 400; // Menos que o custo de 500
        size_t initialBulletCount = bullets.size();
        
        superShot.fire(ship, bullets, lowScore);
        
        REQUIRE(lowScore == 400); // Não deve ter mudado
        REQUIRE(bullets.size() == initialBulletCount);
    }
}

TEST_CASE("SuperShot Cooldown", "[SuperShot]") {
    SuperShot superShot;
    
    SECTION("Cooldown prevents rapid firing") {
        superShot.addShot();
        Spaceship ship(sf::Vector2f(400.0f, 300.0f), 0.0f, true);
        std::vector<Bullet> bullets;
        int score = 1000;
        
        // Primeiro disparo deve funcionar
        REQUIRE(superShot.canFire());
        superShot.fire(ship, bullets, score);
        
        // Segundo disparo imediato não deve funcionar
        REQUIRE_FALSE(superShot.canFire());
    }
}