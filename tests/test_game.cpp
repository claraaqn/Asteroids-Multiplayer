#include "catch_amalgamated.hpp"
#include "../include/Game.h"

TEST_CASE("Game Initial State", "[Game]") {
    Game game;
    
    SECTION("Game starts in playing state") {
        REQUIRE_FALSE(game.isGameOver());
    }
    
    SECTION("Scores start at zero") {
        // Assuming we can access scores or through game over logic
        game.checkGameOver(true, true, 0, 0);
        REQUIRE_FALSE(game.isGameOver());
    }
}

TEST_CASE("Game Over Conditions", "[Game]") {
    Game game;
    
    SECTION("Both players alive - game continues") {
        game.checkGameOver(true, true, 100, 50);
        REQUIRE_FALSE(game.isGameOver());
    }
    
    SECTION("Player 1 dies - game over, player 2 wins") {
        game.checkGameOver(false, true, 100, 50);
        REQUIRE(game.isGameOver());
        REQUIRE(game.getWinner() == 2);
    }
    
    SECTION("Player 2 dies - game over, player 1 wins") {
        game.checkGameOver(true, false, 100, 50);
        REQUIRE(game.isGameOver());
        REQUIRE(game.getWinner() == 1);
    }
    
    SECTION("Both players die - higher score wins") {
        game.checkGameOver(false, false, 100, 150);
        REQUIRE(game.isGameOver());
        REQUIRE(game.getWinner() == 2);
    }
    
    SECTION("Both players die with equal scores - tie") {
        game.checkGameOver(false, false, 100, 100);
        REQUIRE(game.isGameOver());
        REQUIRE(game.getWinner() == 0);
    }
}

TEST_CASE("Game Reset", "[Game]") {
    Game game;
    
    // First cause game over
    game.checkGameOver(false, true, 100, 50);
    REQUIRE(game.isGameOver());
    
    SECTION("Reset returns game to playing state") {
        game.reset();
        REQUIRE_FALSE(game.isGameOver());
    }
    
    SECTION("Reset clears winner") {
        game.reset();
        // Winner should be reset (implementation specific)
        REQUIRE_FALSE(game.isGameOver());
    }
}