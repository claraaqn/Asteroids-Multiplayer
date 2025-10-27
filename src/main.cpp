// main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Menu.h"
#include "GameSession.h"
#include "GameConstants.h"
#include "NameInputScreen.h"

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

    if (screenAspectRatio > gameAspectRatio) {
        float newWidth = HEIGHT * screenAspectRatio;
        gameView.setSize(newWidth, HEIGHT);
    } else {
        float newHeight = WIDTH / screenAspectRatio;
        gameView.setSize(WIDTH, newHeight);
    }
    gameView.setViewport(sf::FloatRect(0, 0, 1, 1));

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
    std::string playerName = "";

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
        
        if (selectedMode == GameMode::None) {
            menu.draw();
        } else {
            break;
        }
    }

    NameInputScreen nameInputScreen(window, font);

    std::string player1Name;
    std::string player2Name;

    if (selectedMode == GameMode::SinglePlayer || selectedMode == GameMode::Multiplayer) {
        nameInputScreen.activate(selectedMode);
        
        while (window.isOpen() && nameInputScreen.isActive()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    break;
                }
                nameInputScreen.handleEvent(event);
            }
            
            // Saiu da tela → pega os nomes
            if (!nameInputScreen.isActive()) {
                if (selectedMode == GameMode::SinglePlayer) {
                    player1Name = nameInputScreen.getPlayer1Name();
                    if (player1Name.empty()) player1Name = "Player";
                } 
                else if (selectedMode == GameMode::Multiplayer) {
                    player1Name = nameInputScreen.getPlayer1Name();
                    player2Name = nameInputScreen.getPlayer2Name();

                    if (player1Name.empty()) player1Name = "Player 1";
                    if (player2Name.empty()) player2Name = "Player 2";
                }
                break;
            }
            
            window.clear(sf::Color::Black);
            nameInputScreen.draw();
            window.display();
        }
    }

    try {
        if (selectedMode == GameMode::SinglePlayer || selectedMode == GameMode::Multiplayer) {
            std::cout << "Iniciando jogo no modo: " 
                    << (selectedMode == GameMode::SinglePlayer ? "SinglePlayer" : "Multiplayer") 
                    << " com jogador: " << playerName << std::endl;
            
            window.setView(gameView);
            GameSession gameSession(window, font, selectedMode, gameView, hudView);
            
            if (selectedMode == GameMode::SinglePlayer) {
                gameSession.setPlayerName(player1Name);
            } else {
                gameSession.setPlayerName(player1Name, player2Name);
            }
            
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