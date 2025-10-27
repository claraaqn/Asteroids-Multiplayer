#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GameMode.h"

class NameInputScreen {
private:
    sf::RenderWindow& window;
    sf::Font& font;
    GameMode gameMode;

    std::string player1Name;
    std::string player2Name;

    int currentPlayer;

    sf::Text titleText;
    sf::Text inputText;
    sf::Text promptText;
    sf::Text startText;
    bool m_isActive;
    
public:
    NameInputScreen(sf::RenderWindow& window, sf::Font& font);
    
    void activate(GameMode mode);
    void deactivate();
    bool isActive() const;

    std::string getPlayer1Name() const;
    std::string getPlayer2Name() const;
    
    void handleEvent(sf::Event& event);
    void draw();
};