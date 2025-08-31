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
                break;
            }
            
            GameMode newMode = menu.handleInput(event);
            if (newMode != GameMode::None) {
                selectedMode = newMode;
                break; 
            }
        }
        
        //! TEM ERRO SÓ DESENHA SE AINDA ESTIVER NO MENU
        if (selectedMode == GameMode::None) {
            menu.draw();
        } else {
            break; // SAIR DO LOOP PRINCIPAL DO MENU
        }
    }

    std::cout << "Modo selecionado: " << static_cast<int>(selectedMode) << std::endl;

    //? 5. Execução do Jogo - VOLTE PARA A VERSÃO SIMPLES
    try {
        if (selectedMode == GameMode::SinglePlayer || selectedMode == GameMode::Multiplayer) {
            std::cout << "Iniciando jogo no modo: " 
                    << (selectedMode == GameMode::SinglePlayer ? "SinglePlayer" : "Multiplayer") 
                    << std::endl;
            
            window.setView(gameView);
            GameSession gameSession(window, font, selectedMode);
            
            std::cout << "GameSession criada, executando run()..." << std::endl;
            gameSession.run();
            
            std::cout << "GameSession finalizada" << std::endl;
        } else if (selectedMode == GameMode::Exit) {
            std::cout << "Saindo do jogo..." << std::endl;
            window.close();
        }
    } catch (const std::exception& e) {
    std::cerr << "EXCEÇÃO: " << e.what() << std::endl;
    return EXIT_FAILURE;
    }
    
    return 0;
}