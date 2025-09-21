#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class NameInputScreen {
private:
    sf::RenderWindow& window;
    sf::Font& font;
    std::string playerName;
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
    std::string getPlayerName() const;
    
    void handleEvent(sf::Event& event);
    void draw();
};