// main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Menu.h"
#include "GameSession.h"
#include "GameConstants.h"

using namespace GameConstants;

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    //? 1. Configuração da Janela e Recursos em tela cheia
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    
    sf::RenderWindow window(desktop, "Asteroids", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    //? 1.2 Configurar o View para a proporção
    sf::View gameView;
    gameView.setSize(WIDTH, HEIGHT);
    gameView.setCenter(WIDTH / 2.0f, HEIGHT / 2.0f);

    //? 2 Calculo do View Port
    float screenWidth = desktop.width;
    float screenHeight = desktop.height;

    float screenAspectRatio = screenWidth / screenHeight;
    float gameAspectRatio = (float)WIDTH / (float)HEIGHT;

    float viewportX, viewportY, viewportWidth, viewportHeight;

    if (screenAspectRatio > gameAspectRatio) {
        viewportWidth = gameAspectRatio / screenAspectRatio;
        viewportHeight = 1.0f;
        viewportX = (1.0f - viewportWidth) / 2.0f;
        viewportY = 0.0f;
    } else {
        viewportWidth = 1.0f;
        viewportHeight = screenAspectRatio / gameAspectRatio;
        viewportX = 0.0f;
        viewportY = (1.0f - viewportHeight) / 2.0f;
    }
    gameView.setViewport(sf::FloatRect(viewportX, viewportY, viewportWidth, viewportHeight));


    //? 3. CARREGA A FONTE
    sf::Font font;
    if (!font.loadFromFile("assets\\font\\PixelifySans-Regular.ttf")) {
        std::cerr << "Arquivo de fonte não encontrado!" << std::endl;
        return EXIT_FAILURE;
    }

    //? 4. Loop do Menu
    Menu menu(window, font);
    GameMode selectedMode = GameMode::None;

    window.setView(window.getDefaultView());
    while (window.isOpen() && selectedMode == GameMode::None) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            selectedMode = menu.handleInput(event);
        }
        menu.draw();
    }

    //? 5. Execução do Jogo
    if (selectedMode == GameMode::SinglePlayer || selectedMode == GameMode::Multiplayer) {
        window.setView(gameView);
        GameSession gameSession(window, font, selectedMode);
        gameSession.run();
    }
    
    return 0;
}