#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"

class Menu {
public:
    Menu(sf::RenderWindow& window, sf::Font& font);
    void draw();
    bool handleInput(sf::Event& event);
    
private:
    sf::RenderWindow& window;
    sf::Font& font;
    sf::Text titleText;
    sf::Text playText;
    sf::Text exitText;
    sf::Text creditsText;
};