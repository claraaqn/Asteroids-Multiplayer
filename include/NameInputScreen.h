#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class NameInputScreen {
private:
    sf::RenderWindow& window;
    sf::Font& font;

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
    
    void activate();
    void deactivate();
    bool isActive() const;

    std::string getPlayer1Name() const;
    std::string getPlayer2Name() const;
    
    void handleEvent(sf::Event& event);
    void draw();
};