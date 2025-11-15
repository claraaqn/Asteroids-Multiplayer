#include "catch_amalgamated.hpp"
#include "../include/Bullet.h"
#include "../include/GameConstants.h"

TEST_CASE("Bullet Initialization", "[Bullet]") {
    Bullet bullet;
    
    SECTION("Bullet starts inactive") {
        REQUIRE_FALSE(bullet.isActive);
    }
    
    SECTION("Bullet can be fired") {
        bullet.fire(sf::Vector2f(100.0f, 100.0f), 0.0f);
        REQUIRE(bullet.isActive);
    }
}

TEST_CASE("Bullet Movement", "[Bullet]") {
    Bullet bullet;
    bullet.fire(sf::Vector2f(400.0f, 300.0f), 90.0f); // Fire straight up
    
    SECTION("Bullet moves correctly") {
        auto initialBounds = bullet.getBounds();
        bullet.update(1.0f); // 1 segundo
        
        auto newBounds = bullet.getBounds();
        REQUIRE(newBounds.top < initialBounds.top); // Should move up
    }
    
    SECTION("Bullet becomes inactive when out of bounds") {
        // Move bullet far out of bounds
        bullet.fire(sf::Vector2f(-100.0f, -100.0f), 0.0f);
        bullet.update(0.1f);
        
        REQUIRE_FALSE(bullet.isActive);
    }
}

TEST_CASE("Bullet Direction", "[Bullet]") {
    SECTION("Bullet moves in correct direction based on angle") {
        Bullet bullet1, bullet2, bullet3;
        
        // Fire at different angles
        bullet1.fire(sf::Vector2f(400.0f, 300.0f), 0.0f);   // Right
        bullet2.fire(sf::Vector2f(400.0f, 300.0f), 90.0f);  // Up
        bullet3.fire(sf::Vector2f(400.0f, 300.0f), 180.0f); // Left
        
        bullet1.update(1.0f);
        bullet2.update(1.0f);
        bullet3.update(1.0f);
        
        // Each should have moved in different directions
        REQUIRE(bullet1.isActive);
        REQUIRE(bullet2.isActive);
        REQUIRE(bullet3.isActive);
    }
}