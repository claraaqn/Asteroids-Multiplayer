#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <SFML/Graphics.hpp>
#include "Game.h"

class GameOverScreen {
public:
    GameOverScreen(const sf::Font& font);
    
    void update(int winner, int score1, int score2);
    void draw(sf::RenderWindow& window);
    void setPosition(float x, float y);

private:
    sf::Text gameOverText;
    sf::Text winnerText;
    sf::Text restartText;
    sf::Text quitText;
    
    void centerText(sf::Text& text);
};

#endif