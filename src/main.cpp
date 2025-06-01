#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>    // Para std::to_string
#include <cstdlib>   // Para rand(), srand()
#include <ctime>     // Para time()
#include <SFML/Audio.hpp>
#include <iostream>

#include "Spaceship.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "Game.h"
#include "GameConstants.h"

using namespace GameConstants; 

int main() {

    srand(static_cast<unsigned int>(time(NULL)));
    sf::Listener::setGlobalVolume(100);


    // Verifica se há joysticks conectados
    if (sf::Joystick::isConnected(0)) {
        std::cout << "Joystick 0 conectado!" << std::endl;
    }
    if (sf::Joystick::isConnected(1)) {
        std::cout << "Joystick 1 conectado!" << std::endl;
    }

    srand(static_cast<unsigned int>(time(NULL)));
    
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    // Use sf::Style::None para tela cheia sem bordas
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height), "Asteroids Multiplayer", sf::Style::None); 
    window.setFramerateLimit(60);
    window.setPosition(sf::Vector2i(0, 0)); 
    window.setMouseCursorVisible(false); // Opcional, esconde o cursor

    // --- CONFIGURAÇÃO DA sf::View PARA PREENCHER A TELA (STRETCH TO FILL) ---
    sf::View gameView(sf::FloatRect(0, 0, WIDTH, HEIGHT)); // Sua resolução de jogo "virtual"
    gameView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f)); // Ocupa 100% da janela
    window.setView(gameView);
    
    sf::Font font;
    if (!font.loadFromFile("PixelifySans-Regular.ttf")) { // Certifique-se que arial.ttf está na pasta do executável
        return EXIT_FAILURE;
    }

    // --- CARREGAMENTO DE EFEITOS SONOROS ---
    sf::SoundBuffer shootBuffer;
    if (!shootBuffer.loadFromFile("assets/laser1.wav")) {
        // Handle error: could not load audio file
        return EXIT_FAILURE;
    }

    sf::SoundBuffer explosionBuffer;
    if (!explosionBuffer.loadFromFile("assets/explosion.wav")) {
        // Handle error: could not load audio file
        return EXIT_FAILURE;
    }

    // --- TEXTO DE GAME OVER ÚNICO (APARECE QUANDO QUALQUER UM MORRE) ---
    sf::Text gameOverText; // Texto centralizado de Game Over
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER!");
    gameOverText.setCharacterSize(60); 
    gameOverText.setFillColor(sf::Color::White); 
    gameOverText.setOrigin(gameOverText.getLocalBounds().width/2, gameOverText.getLocalBounds().height/2);
    gameOverText.setPosition(WIDTH/2, HEIGHT/2 - 50);

    // --- TEXTO DE REINÍCIO GLOBAL (APARECE JUNTO COM O GAME OVER) ---
    sf::Text restartText; 
    restartText.setFont(font);
    restartText.setString("PRESS 'R' TO RESTART");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);
    restartText.setOrigin(restartText.getLocalBounds().width/2, restartText.getLocalBounds().height/2);
    restartText.setPosition(WIDTH/2, HEIGHT/2 + 50); 
    // --- FIM DOS TEXTOS DE GAME OVER ---

    Game game; // Instância do gerenciador de estado do jogo

    sf::RectangleShape divider(sf::Vector2f(2, HEIGHT));
    divider.setFillColor(sf::Color::White);
    divider.setPosition(WIDTH/2, 0);

    Spaceship player1(sf::Vector2f(WIDTH/4, HEIGHT - 40), 0, true);
    player1.shape.setOutlineColor(sf::Color::Green);

    Spaceship player2(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);
    player2.shape.setOutlineColor(sf::Color::Cyan);

    std::vector<Bullet> bullets1(10);
    std::vector<Bullet> bullets2(10);
    std::vector<sf::Sound> activeSounds;

    for (auto& bullet : bullets1) { bullet.shape.setFillColor(sf::Color::Green); }
    for (auto& bullet : bullets2) { bullet.shape.setFillColor(sf::Color::Cyan); }

    std::vector<Asteroid> asteroids;
    sf::Clock asteroidClock;
    
    sf::Text scoreText1; 
    scoreText1.setFont(font);
    scoreText1.setCharacterSize(20);
    scoreText1.setFillColor(sf::Color::Green);
    scoreText1.setPosition(10, 10);

    sf::Text scoreText2; 
    scoreText2.setFont(font);
    scoreText2.setCharacterSize(20);
    scoreText2.setFillColor(sf::Color::Cyan);
    scoreText2.setPosition(WIDTH - 100, 10);

    int score1 = 0; 
    int score2 = 0;

    while (window.isOpen()) {
        activeSounds.erase(std::remove_if(activeSounds.begin(), activeSounds.end(), 
                                  [](const sf::Sound& s){ return s.getStatus() == sf::Sound::Stopped; }), 
                   activeSounds.end());

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::JoystickConnected) {
                std::cout << "Joystick " << event.joystickConnect.joystickId << " conectado!" << std::endl;
            }
            if (event.type == sf::Event::JoystickDisconnected) {
                std::cout << "Joystick " << event.joystickConnect.joystickId << " desconectado!" << std::endl;
            }


            // --- Lógica de Reinício do Jogo (ativada se o jogo está em GAME_OVER) ---
            if (game.isGameOver() && (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) || 
    (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.button == 7)) {

                if (event.key.code == sf::Keyboard::R) {
                    game.reset(); 

                    player1 = Spaceship(sf::Vector2f(WIDTH/4, HEIGHT - 40), 0, true);
                    player1.shape.setOutlineColor(sf::Color::Green);
                    
                    player2 = Spaceship(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);
                    player2.shape.setOutlineColor(sf::Color::Cyan);
                    
                    bullets1.clear(); bullets1.resize(10);
                    for (auto& bullet : bullets1) bullet.shape.setFillColor(sf::Color::Green);
                    bullets2.clear(); bullets2.resize(10);
                    for (auto& bullet : bullets2) bullet.shape.setFillColor(sf::Color::Cyan);
                    
                    asteroids.clear();
                    // Aqui, as variáveis x, y, vx, vy precisam ser declaradas novamente
                    for (int i = 0; i < 5; ++i) {
                        float x = rand() % WIDTH; 
                        float y = rand() % (HEIGHT - 100); 
                        float vx = (rand() % 100) / 50.0f - 1.0f; 
                        float vy = (rand() % 100) / 50.0f - 1.0f; 
                        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), 3);
                    }
                    score1 = 0; score2 = 0; 
                }
            }
            // --- Fim da Lógica de Reinício do Jogo ---

            // Sistema de tiro por evento (se a nave estiver viva)
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && player1.canFire() && player1.isAlive) {
                    for (auto& bullet : bullets1) {
                        if (!bullet.isActive) { 
                            bullet.fire(player1.getFirePosition(), player1.angle); 
                            player1.resetFireCooldown(); 
                            
                           
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(shootBuffer);
                            activeSounds.back().setVolume(70); // Aumente o volume para teste
                            activeSounds.back().play(); // <--- ADICIONE AO VETOR!
                            break;  
                        }
                    }
                }
                if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) && 
                    player2.canFire() && player2.isAlive){               
                        for (auto& bullet : bullets2) {
                            if (!bullet.isActive) { 
                                 bullet.fire(player2.getFirePosition(), player2.angle); 
                                player2.resetFireCooldown(); 
                               
                                
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(shootBuffer);
                            activeSounds.back().setVolume(70); // Aumente o volume para teste
                            activeSounds.back().play(); // <--- ADICIONE AO VETOR!
                           
                                break; 
                            }
                        }
                }
            }
        } // Fim do while (window.pollEvent(event))
        
        //? --- CONTROLES E ATUALIZAÇÕES DO JOGO (somente se não for Game Over global) ---
        if (!game.isGameOver()) { 
            // Controles só para jogadores vivos
            // Controles do jogador 1 (Joystick 0)
            if (player1.isAlive) {
                // Eixo X do analógico esquerdo para virar
                float joystickX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
                if (std::abs(joystickX) > 25.0f) {  // Deadzone de 25%
                    player1.angle += joystickX * 0.085f;  // Ajuste a sensibilidade conforme necessário
                }

                // Gatilho direito para acelerar
                float trigger = sf::Joystick::getAxisPosition(0, sf::Joystick::Z) / 100.0f;
                if (trigger > 0.065f) {
                    player1.accelerate(trigger * 0.1f);
                }

                // Botão A (0) para atirar
                if (sf::Joystick::isButtonPressed(0, 0) && player1.canFire()) {
                    for (auto& bullet : bullets1) {
                        if (!bullet.isActive) { 
                            bullet.fire(player1.getFirePosition(), player1.angle); 
                            player1.resetFireCooldown();
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(shootBuffer);
                            activeSounds.back().setVolume(70); // Aumente o volume para teste
                            activeSounds.back().play(); 
                            break; 
                        }
                    }
                }

                player1.decelerate();
                player1.update();
            }

            // Controles do jogador 2 (Joystick 1)
            if (player2.isAlive) {
                // Eixo X do analógico esquerdo para virar
                float joystickX = sf::Joystick::getAxisPosition(1, sf::Joystick::X);
                if (std::abs(joystickX) > 25.0f) {  // Deadzone de 25%
                    player2.angle += joystickX * 0.1f;
                }

                // Gatilho direito para acelerar
                float trigger = sf::Joystick::getAxisPosition(1, sf::Joystick::Z) / 100.0f;
                if (trigger > 0.1f) {
                    player2.accelerate(trigger * 0.065f);
                }

                // Botão A (0) para atirar
                if (sf::Joystick::isButtonPressed(1, 0) && player2.canFire()) {
                    for (auto& bullet : bullets2) {
                        if (!bullet.isActive) { 
                            bullet.fire(player2.getFirePosition(), player2.angle); 
                            player2.resetFireCooldown(); 
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(shootBuffer);
                            activeSounds.back().setVolume(70); // Aumente o volume para teste
                            activeSounds.back().play();
                            break; 
                        }
                    }
                }

                player2.decelerate();
                player2.update();
            }

            // Balas e Asteroides sempre atualizam enquanto o jogo está ativo (não Game Over geral)
            for (auto& bullet : bullets1) bullet.update();
            for (auto& bullet : bullets2) bullet.update();
            for (auto& asteroid : asteroids) asteroid.update();

            //! SPAWN DE NOVOS ASTEROIDES
            if (asteroidClock.getElapsedTime().asSeconds() > 1.5f) {
                float x = rand() % WIDTH;
                float y = -50;
                float vx = (rand() % 100) / 100.0f - 0.5f;
                float vy = 1.0f + (rand() % 100) / 25.0f;
                asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), 3);
                asteroidClock.restart();
            }

            //! Spawn de novos asteroides (só enquanto o jogo não for Game Over geral)
            for (size_t i = 0; i < asteroids.size(); ) {
                asteroids[i].update();
                
                if (asteroids[i].getPosition().y > HEIGHT + 50) {
                    // Recicla o asteroide
                    float newX = rand() % WIDTH;
                    float newY = -50;
                    float newVx = (rand() % 100) / 100.0f - 0.5f;
                    float newVy = 1.0f + (rand() % 100) / 25.0f;
                    
                    asteroids[i] = Asteroid(sf::Vector2f(newX, newY), sf::Vector2f(newVx, newVy), 3);
                    i++;
                }
                else {
                    i++;
                }
            }

            // --- Colisões (apenas se a nave estiver viva) ---
            if (player1.isAlive) {
                for (auto& bullet : bullets1) {
                    if (!bullet.isActive) continue;
                    for (size_t i = 0; i < asteroids.size(); ++i) {
                        sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                        if (distance < asteroids[i].getRadius()) {
                            bullet.isActive = false; score1 += (4 - asteroids[i].size) * 10;
                            if (asteroids[i].size > 1) {
                                for (int j = 0; j < 2; ++j) {
                                    float angle = rand() % 360 * PI / 180; float speed = 1 + (rand() % 100) / 50.0f;
                                    sf::Vector2f vel(speed * std::cos(angle), speed * std::sin(angle));
                                    asteroids.emplace_back(asteroids[i].getPosition(), vel, asteroids[i].size - 1);
                                }
                            }
                            asteroids.erase(asteroids.begin() + i);
                            // Adicione este código após apagar o asteroide
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(explosionBuffer);
                            activeSounds.back().setVolume(70);
                            activeSounds.back().play(); break;
                        }
                    }
                }
                // Colisão nave-asteroide para Player 1
                for (const auto& asteroid : asteroids) {
                    sf::Vector2f diff = player1.position - asteroid.getPosition();
                    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    if (distance < asteroid.getRadius() + 20) { player1.isAlive = false; break; }
                }
            }

            if (player2.isAlive) {
                for (auto& bullet : bullets2) {
                    if (!bullet.isActive) continue;
                    for (size_t i = 0; i < asteroids.size(); ++i) {
                        sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                        if (distance < asteroids[i].getRadius()) {
                            bullet.isActive = false; score2 += (4 - asteroids[i].size) * 10;
                            if (asteroids[i].size > 1) {
                                for (int j = 0; j < 2; ++j) {
                                    float angle = rand() % 360 * PI / 180; float speed = 1 + (rand() % 100) / 50.0f;
                                    sf::Vector2f vel(speed * std::cos(angle), std::sin(angle));
                                    asteroids.emplace_back(asteroids[i].getPosition(), vel, asteroids[i].size - 1);
                                }
                            }
                            asteroids.erase(asteroids.begin() + i); 
                            
                            // Adicione este código após apagar o asteroide
                            
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(explosionBuffer);
                            activeSounds.back().setVolume(70);
                            activeSounds.back().play();
                            break;
                        }
                    }
                }
                // Colisão nave-asteroide para Player 2
                for (const auto& asteroid : asteroids) {
                    sf::Vector2f diff = player2.position - asteroid.getPosition();
                    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    if (distance < asteroid.getRadius() + 20) { player2.isAlive = false; break; }
                }
            }
            // --- Fim das Colisões ---

            // A classe Game verifica o estado geral:
            // game.checkGameOver é chamado para ver se QUALQUER um dos players morreu
            game.checkGameOver(player1.isAlive, player2.isAlive); 

            scoreText1.setString("P1: " + std::to_string(score1));
            scoreText2.setString("P2: " + std::to_string(score2));
        } // Fim do 'if (!game.isGameOver())' para updates de jogo

        // --- Renderização ---
        window.clear();

        window.draw(divider);

        // Desenha asteroides, balas, naves se estiverem ativas
        for (const auto& asteroid : asteroids) { window.draw(asteroid.shape); }

        // Naves só são desenhadas se estiverem vivas
        if (player1.isAlive) window.draw(player1.shape);
        if (player2.isAlive) window.draw(player2.shape);

        // Balas sempre são desenhadas se ativas, independentemente do player estar vivo
        for (const auto& bullet : bullets1) { if (bullet.isActive) window.draw(bullet.shape); }
        for (const auto& bullet : bullets2) { if (bullet.isActive) window.draw(bullet.shape); }

        window.draw(scoreText1);
        window.draw(scoreText2);

        // --- Desenha mensagens de Game Over INDIVIDUALMENTE ---
    

        // --- Desenha o texto de reinício GLOBAL se o jogo estiver em GAME_OVER ---
        if (game.isGameOver()) { // game.isGameOver() é TRUE se QUALQUER jogador morreu
            window.draw(gameOverText); // Mensagem central "GAME OVER!"
            window.draw(restartText);  // Mensagem central "PRESS 'R' TO RESTART"
        }

        window.display();
    }

    return 0;
}