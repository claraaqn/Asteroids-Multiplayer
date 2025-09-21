#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Game.h"
#include "HighScoreDB.h"
#include "GameMode.h"

class GameOverScreen {
public:
    GameOverScreen(const sf::Font& font, HighScoreDB& highScoreDB);
    
    void update(int winner, int score1, int score2, GameMode gameMode);
    void draw(sf::RenderWindow& window);
    void setPosition(float x, float y);
    void refreshHighScores(GameMode mode);

private:
    sf::Text gameOverText;
    sf::Text winnerText;
    sf::Text restartText;
    sf::Text quitText;
    sf::Text highScoresTitle;
    std::vector<sf::Text> highScoreTexts;
    
    HighScoreDB& highScoreDB;
    
    void centerText(sf::Text& text);
    void updateHighScoresDisplay();
};

#endif