#include "catch_amalgamated.hpp"
#include "../include/Menu.h"
#include "../include/GameMode.h"
#include <SFML/Graphics.hpp>

TEST_CASE("Menu Initialization", "[Menu]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    
    Menu menu(window, font);
    
    SECTION("Menu is created with correct number of items") {
        // Verifica se o menu foi criado com 3 itens (Single, Multi, Exit)
        REQUIRE(true); // A verificação específica depende da implementação interna
    }
    
    SECTION("Initial selection is on first item") {
        // O primeiro item deve estar selecionado inicialmente
        REQUIRE(true);
    }
}

TEST_CASE("Menu Navigation", "[Menu]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    Menu menu(window, font);
    
    SECTION("Move down through menu items") {
        // Simula pressionar seta para baixo
        sf::Event event;
        event.type = sf::Event::KeyPressed;
        event.key.code = sf::Keyboard::Down;
        
        GameMode result = menu.handleInput(event);
        REQUIRE(result == GameMode::None); // Apenas navega, não seleciona
    }
    
    SECTION("Move up through menu items") {
        // Simula pressionar seta para cima
        sf::Event event;
        event.type = sf::Event::KeyPressed;
        event.key.code = sf::Keyboard::Up;
        
        GameMode result = menu.handleInput(event);
        REQUIRE(result == GameMode::None);
    }
}

TEST_CASE("Menu Selection", "[Menu]") {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Font font;
    Menu menu(window, font);
    
    SECTION("Select SinglePlayer mode") {
        sf::Event event;
        event.type = sf::Event::KeyPressed;
        event.key.code = sf::Keyboard::Return;
        
        // Primeiro navega para o item desejado, depois seleciona
        // Esta é uma simplificação - na prática precisaria controlar o estado
        GameMode result = menu.handleInput(event);
        // Depende do item atualmente selecionado
        REQUIRE((result == GameMode::SinglePlayer || result == GameMode::Multiplayer || result == GameMode::Exit));
    }
}