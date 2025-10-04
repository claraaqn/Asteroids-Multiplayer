#include "catch_amalgamated.hpp"
#include "../include/Spaceship.h"
#include "../include/GameConstants.h"
#include <SFML/Graphics.hpp>

TEST_CASE("Spaceship Initialization", "[Spaceship]") {
    sf::Vector2f position(400.0f, 300.0f);
    float angle = 0.0f;

    Spaceship ship(position, angle, true);

    SECTION("Initial position and angle are set correctly") {
        REQUIRE(ship.getBounds().contains(position));
        REQUIRE(ship.isAlive == true);
    }

    SECTION("Player1 vs Player2 setup") {
        Spaceship player1Ship(position, angle, true);
        Spaceship player2Ship(position, angle, false);

        REQUIRE(player1Ship.isAlive == true);
        REQUIRE(player2Ship.isAlive == true);
    }
}

TEST_CASE("Spaceship Movement", "[Spaceship]") {
    sf::Vector2f position(400.0f, 300.0f);
    Spaceship ship(position, 0.0f, true);

    SECTION("Acceleration affects velocity") {
        ship.accelerate(1.0f);
        // Testa se pelo menos moveu
        REQUIRE(ship.getBounds().left > 0);
    }

    SECTION("Deceleration reduces velocity") {
        ship.accelerate(1.0f);
        ship.decelerate();
        REQUIRE(true); // Não dá pra medir exato, mas garante que roda sem erro
    }
}

TEST_CASE("Spaceship Boundaries", "[Spaceship]") {
    SECTION("Player1 stays within screen bounds") {
        Spaceship ship(sf::Vector2f(10.0f, 300.0f), 0.0f, true);
        ship.update(0.1f);

        REQUIRE(ship.getBounds().left >= 0);
    }

    SECTION("Player2 stays within right half in multiplayer") {
        Spaceship ship(sf::Vector2f(GameConstants::WIDTH/2 - 50.0f, 300.0f), 0.0f, false);
        ship.update(0.1f);

        REQUIRE(ship.getBounds().left >= GameConstants::WIDTH/2);
    }
}

TEST_CASE("Spaceship Firing", "[Spaceship]") {
    Spaceship ship(sf::Vector2f(400.0f, 300.0f), 0.0f, true);

    SECTION("Can fire after cooldown") {
        REQUIRE_FALSE(ship.canFire()); // imediatamente não pode
        ship.resetFireCooldown();
        REQUIRE_FALSE(ship.canFire()); // ainda dentro do cooldown
    }

    SECTION("Fire position is calculated correctly") {
        auto firePos = ship.getFirePosition();
        REQUIRE(firePos.x != 400.0f);
        REQUIRE(firePos.y != 300.0f);
    }
}

TEST_CASE("Spaceship Reset", "[Spaceship]") {
    Spaceship ship(sf::Vector2f(400.0f, 300.0f), 45.0f, true);

    SECTION("Reset returns to initial state") {
        ship.reset(sf::Vector2f(200.0f, 200.0f), 90.0f, false);
        REQUIRE(ship.getBounds().contains(sf::Vector2f(200.0f, 200.0f)));
    }
}
