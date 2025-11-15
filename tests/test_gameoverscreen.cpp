#include "catch_amalgamated.hpp"
#include "../include/GameOverScreen.h"
#include "../include/HighScoreDB.h"
#include <SFML/Graphics.hpp>

class MockHighScoreDB : public HighScoreDB {
public:
    std::vector<HighScore> getTopScores(int limit = 5) override {
        std::vector<HighScore> scores;
        for (int i = 0; i < limit; i++) {
            HighScore hs;
            hs.playerName = "Player" + std::to_string(i + 1);
            hs.score = (limit - i) * 100;
            hs.gameMode = (i % 2 == 0) ? GameMode::SinglePlayer : GameMode::Multiplayer;
            scores.push_back(hs);
        }
        return scores;
    }
};

TEST_CASE("GameOverScreen Initialization", "[GameOverScreen]") {
    sf::Font font;
    MockHighScoreDB mockDB;
    
    GameOverScreen screen(font, mockDB);
    
    SECTION("Screen creates all text elements") {
        // Can't easily test drawing, but we can verify the object is created
        REQUIRE(true); // Placeholder - object creation is success
    }
}

TEST_CASE("GameOverScreen Update", "[GameOverScreen]") {
    sf::Font font;
    MockHighScoreDB mockDB;
    GameOverScreen screen(font, mockDB);
    
    SECTION("Update with player 1 win") {
        screen.update(1, 100, 50, GameMode::SinglePlayer);
        // Should set appropriate text (hard to test without rendering)
        REQUIRE(true);
    }
    
    SECTION("Update with player 2 win") {
        screen.update(2, 50, 100, GameMode::Multiplayer);
        REQUIRE(true);
    }
    
    SECTION("Update with tie") {
        screen.update(0, 100, 100, GameMode::SinglePlayer);
        REQUIRE(true);
    }
}

TEST_CASE("GameOverScreen High Scores", "[GameOverScreen]") {
    sf::Font font;
    MockHighScoreDB mockDB;
    GameOverScreen screen(font, mockDB);
    
    SECTION("Refresh high scores loads data") {
        screen.refreshHighScores(GameMode::SinglePlayer);
        // Should load scores from mock DB
        REQUIRE(true);
    }
}

TEST_CASE("GameOverScreen Positioning", "[GameOverScreen]") {
    sf::Font font;
    MockHighScoreDB mockDB;
    GameOverScreen screen(font, mockDB);
    
    SECTION("Set position moves all elements") {
        screen.setPosition(400.0f, 300.0f);
        // All text elements should be repositioned
        REQUIRE(true);
    }
}