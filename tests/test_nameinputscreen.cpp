#include "catch_amalgamated.hpp"
#include "../include/NameInputScreen.h"
#include <SFML/Graphics.hpp>

TEST_CASE("NameInputScreen Initialization", "[NameInputScreen]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    
    NameInputScreen nameScreen(window, font);
    
    SECTION("Screen starts inactive") {
        REQUIRE_FALSE(nameScreen.isActive());
    }
    
    SECTION("Player name starts empty") {
        REQUIRE(nameScreen.getPlayerName().empty());
    }
}

TEST_CASE("NameInputScreen Activation", "[NameInputScreen]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    NameInputScreen nameScreen(window, font);
    
    SECTION("Activate makes screen active") {
        nameScreen.activate();
        REQUIRE(nameScreen.isActive());
    }
    
    SECTION("Deactivate makes screen inactive") {
        nameScreen.activate();
        nameScreen.deactivate();
        REQUIRE_FALSE(nameScreen.isActive());
    }
}

TEST_CASE("NameInputScreen Text Input", "[NameInputScreen]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    NameInputScreen nameScreen(window, font);
    nameScreen.activate();
    
    SECTION("Add characters to name") {
        sf::Event event;
        event.type = sf::Event::TextEntered;
        event.text.unicode = 'A';
        
        nameScreen.handleEvent(event);
        REQUIRE(nameScreen.getPlayerName() == "A");
    }
    
    SECTION("Backspace removes characters") {
        // Primeiro adiciona um caractere
        sf::Event addEvent;
        addEvent.type = sf::Event::TextEntered;
        addEvent.text.unicode = 'A';
        nameScreen.handleEvent(addEvent);
        
        // Depois backspace
        sf::Event backEvent;
        backEvent.type = sf::Event::TextEntered;
        backEvent.text.unicode = '\b';
        nameScreen.handleEvent(backEvent);
        
        REQUIRE(nameScreen.getPlayerName().empty());
    }
    
    SECTION("Enter deactivates when name is not empty") {
        // Adiciona um caractere
        sf::Event addEvent;
        addEvent.type = sf::Event::TextEntered;
        addEvent.text.unicode = 'A';
        nameScreen.handleEvent(addEvent);
        
        // Pressiona Enter
        sf::Event enterEvent;
        enterEvent.type = sf::Event::TextEntered;
        enterEvent.text.unicode = '\r';
        nameScreen.handleEvent(enterEvent);
        
        REQUIRE_FALSE(nameScreen.isActive());
    }
    
    SECTION("Enter does nothing when name is empty") {
        // Pressiona Enter sem nome
        sf::Event enterEvent;
        enterEvent.type = sf::Event::TextEntered;
        enterEvent.text.unicode = '\r';
        nameScreen.handleEvent(enterEvent);
        
        REQUIRE(nameScreen.isActive()); // Continua ativo
    }
}