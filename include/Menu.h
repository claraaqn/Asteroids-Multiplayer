#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"
#include <iostream>

enum class GameMode {
    None, 
    SinglePlayer, 
    Multiplayer, 
    Exit
};

class Menu {
public:
    Menu(sf::RenderWindow& window, sf::Font& font);
    void draw();
    GameMode handleInput(sf::Event& event);
    
private:
    void moveUp();
    void moveDown();
    void updateMenuText();

    sf::RenderWindow& window;
    sf::Font& font;

    sf::Text titleText;
    sf::Text creditsText;
    
    size_t selectedItemIndex;
    std::vector<sf::Text> menuItems; //Vou utilizar um vetor para as opções do menu

};