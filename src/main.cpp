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
    gameView.setCenter(WIDTH / 2.0f, HEIGHT / 2.0f);


    float screenAspectRatio = (float)desktop.width / (float)desktop.height;
    float gameAspectRatio = (float)WIDTH / (float)HEIGHT;

    // Se a tela for mais larga que o jogo (ex: monitor ultrawide)
    if (screenAspectRatio > gameAspectRatio) {
        // Mantenha a altura do jogo e aumente a largura
        float newWidth = HEIGHT * screenAspectRatio;
        gameView.setSize(newWidth, HEIGHT);
    }
    // Se a tela for mais alta que o jogo (ex: monitor em modo retrato)
    else {
        // Mantenha a largura do jogo e aumente a altura
        float newHeight = WIDTH / screenAspectRatio;
        gameView.setSize(WIDTH, newHeight);
    }
    // O viewport continua sendo 100% da janela, pois não há barras pretas
    gameView.setViewport(sf::FloatRect(0, 0, 1, 1));


    // 3. View para a INTERFACE/HUD (fixa no tamanho da tela)
    sf::View hudView;
    hudView.setSize(desktop.width, desktop.height);
    hudView.setCenter(desktop.width / 2.0f, desktop.height / 2.0f);

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
            GameSession gameSession(window, font, selectedMode, gameView, hudView);
            
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