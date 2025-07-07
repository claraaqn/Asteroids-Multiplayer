// Menu.cpp
#include "Menu.h"

Menu::Menu(sf::RenderWindow& window, sf::Font& font) 
    : window(window), font(font) {
    
    //? Configura o título
    titleText.setFont(font);
    titleText.setString("ASTEROIDS-MULTPLAYER");
    titleText.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 10));
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setOrigin(titleText.getLocalBounds().width/2, titleText.getLocalBounds().height/2);
    titleText.setPosition(window.getSize().x/2, window.getSize().y/3);
    
    //? Configura a opção PLAY
    playText.setFont(font);
    playText.setString("PLAY GAME (P)");
    playText.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 20));
    playText.setFillColor(sf::Color::Green);
    playText.setOrigin(playText.getLocalBounds().width/2, playText.getLocalBounds().height/2);
    playText.setPosition(window.getSize().x/2, window.getSize().y/2);
    
    //? Configura a opção EXIT
    exitText.setFont(font);
    exitText.setString("EXIT (Q)");
    playText.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 20));
    exitText.setFillColor(sf::Color::White);
    exitText.setOrigin(exitText.getLocalBounds().width/2, exitText.getLocalBounds().height/2);
    exitText.setPosition(window.getSize().x/2, window.getSize().y/2 + 60);
    
    //? Configura os créditos
    creditsText.setFont(font);
    creditsText.setString("MENINAS NA CIÊNCIA APRESENTA");
    creditsText.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 30));
    creditsText.setFillColor(sf::Color::White);
    creditsText.setOrigin(creditsText.getLocalBounds().width/2, creditsText.getLocalBounds().height/2);
    creditsText.setPosition(window.getSize().x/2, window.getSize().y - 50);
}

void Menu::draw() 
{
    window.setView(window.getDefaultView());
    window.clear(sf::Color::Black);
    window.draw(titleText);
    window.draw(playText);
    window.draw(exitText);
    window.draw(creditsText);
    window.display();
}

bool Menu::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P) {
            return true; 
        }
        else if (event.key.code == sf::Keyboard::Q) {
            window.close();
        }
    }
    return false;
}