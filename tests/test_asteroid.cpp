#include "catch_amalgamated.hpp"
#include "../include/Asteroid.h"
#include "../include/GameConstants.h"
#include <SFML/Graphics.hpp>

TEST_CASE("Asteroid Initialization", "[Asteroid]") {
    sf::Vector2f position(100.0f, 200.0f);
    sf::Vector2f velocity(50.0f, -30.0f);
    int size = 3;
    
    Asteroid asteroid(position, velocity, size);
    
    SECTION("Position is set correctly") {
        REQUIRE(asteroid.getPosition().x == Catch::Approx(100.0f));
        REQUIRE(asteroid.getPosition().y == Catch::Approx(200.0f));
    }
    
    SECTION("Size is set correctly") {
        REQUIRE(asteroid.getSize() == 3);
    }
    
    SECTION("Radius is calculated based on size") {
        REQUIRE(asteroid.getRadius() > 0.0f);
    }
}

TEST_CASE("Asteroid Movement", "[Asteroid]") {
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f velocity(20.0f, 10.0f);
    Asteroid asteroid(position, velocity, 2);
    
    SECTION("Update changes position") {
        sf::Vector2f initialPos = asteroid.getPosition();
        asteroid.update(1.0f, 0.0f); // 1 segundo
        
        sf::Vector2f newPos = asteroid.getPosition();
        REQUIRE(newPos.x == Catch::Approx(initialPos.x + velocity.x));
        REQUIRE(newPos.y == Catch::Approx(initialPos.y + velocity.y));
    }
    
    SECTION("Multiple updates accumulate movement") {
        sf::Vector2f initialPos = asteroid.getPosition();
        
        // 3 updates de 0.5 segundos cada
        asteroid.update(0.5f, 0.0f);
        asteroid.update(0.5f, 0.0f);
        asteroid.update(0.5f, 0.0f);
        
        sf::Vector2f newPos = asteroid.getPosition();
        REQUIRE(newPos.x == Catch::Approx(initialPos.x + velocity.x * 1.5f));
        REQUIRE(newPos.y == Catch::Approx(initialPos.y + velocity.y * 1.5f));
    }
}

TEST_CASE("Asteroid Scaling", "[Asteroid]") {
    sf::Vector2f position(0.0f, 0.0f);
    sf::Vector2f velocity(0.0f, 0.0f);
    Asteroid asteroid(position, velocity, 2);
    
    float initialRadius = asteroid.getRadius();
    
    SECTION("Scale increases over time") {
        asteroid.update(0.5f, 0.0f);
        float radiusAfterHalf = asteroid.getRadius();
        
        asteroid.update(0.5f, 0.0f);
        float radiusAfterFull = asteroid.getRadius();
        
        REQUIRE(radiusAfterHalf > initialRadius);
        REQUIRE(radiusAfterFull > radiusAfterHalf);
    }
}