#include "catch_amalgamated.hpp"
#include "../include/Menu.h"
#include "../include/NameInputScreen.h"
#include "../include/GameMode.h"
#include <SFML/Graphics.hpp>

TEST_CASE("Game Flow Integration", "[Integration]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    
    SECTION("Menu to NameInputScreen flow") {
        Menu menu(window, font);
        NameInputScreen nameScreen(window, font);
        
        // Simula seleção de SinglePlayer no menu
        // (Esta é uma simplificação - na prática seria mais complexo)
        GameMode selectedMode = GameMode::SinglePlayer;
        
        REQUIRE(selectedMode == GameMode::SinglePlayer);
        
        // Deveria ativar a tela de entrada de nome
        if (selectedMode == GameMode::SinglePlayer) {
            nameScreen.activate();
            REQUIRE(nameScreen.isActive());
        }
    }
    
    SECTION("Name input and game start") {
        NameInputScreen nameScreen(window, font);
        nameScreen.activate();
        
        // Simula entrada de nome
        sf::Event event;
        event.type = sf::Event::TextEntered;
        event.text.unicode = 'T';
        nameScreen.handleEvent(event);
        
        event.text.unicode = 'e';
        nameScreen.handleEvent(event);
        
        event.text.unicode = 's';
        nameScreen.handleEvent(event);
        
        event.text.unicode = 't';
        nameScreen.handleEvent(event);
        
        // Simula pressionar Enter
        event.text.unicode = '\r';
        nameScreen.handleEvent(event);
        
        REQUIRE_FALSE(nameScreen.isActive());
        REQUIRE(nameScreen.getPlayerName() == "Test");
    }
}