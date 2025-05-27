#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>    // Para std::to_string
#include <cstdlib>   // Para rand(), srand()
#include <ctime>     // Para time()

#include "Spaceship.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "Game.h"
#include "GameConstants.h"

using namespace GameConstants; 

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Asteroids Multiplayer");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("PixelifySans-Regular.ttf")) { // Certifique-se que arial.ttf está na pasta do executável
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

    for (auto& bullet : bullets1) { bullet.shape.setFillColor(sf::Color::Green); }
    for (auto& bullet : bullets2) { bullet.shape.setFillColor(sf::Color::Cyan); }

    std::vector<Asteroid> asteroids;
    for (int i = 0; i < 5; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % (HEIGHT - 100);
        float vx = (rand() % 100) / 50.0f - 1.0f;
        float vy = (rand() % 100) / 50.0f - 1.0f;
        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), 3);
    }

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
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // --- Lógica de Reinício do Jogo (ativada se o jogo está em GAME_OVER) ---
            if (game.isGameOver() && event.type == sf::Event::KeyPressed) {
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
                        if (!bullet.isActive) { bullet.fire(player1.getFirePosition(), player1.angle); player1.resetFireCooldown(); break; }
                    }
                }
                if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) && 
                    player2.canFire() && player2.isAlive){               
                        for (auto& bullet : bullets2) {
                            if (!bullet.isActive) { bullet.fire(player2.getFirePosition(), player2.angle); player2.resetFireCooldown(); break; }
                        }
                }
            }
        } // Fim do while (window.pollEvent(event))
        
        // --- CONTROLES E ATUALIZAÇÕES DO JOGO (somente se não for Game Over global) ---
        if (!game.isGameOver()) { 
            // Controles só para jogadores vivos
            if (player1.isAlive) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { player1.angle -= 3.0f; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { player1.angle += 3.0f; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { player1.accelerate(0.1f); }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { player1.accelerate(-0.05f); }
                player1.decelerate();
                player1.update();
            }

            if (player2.isAlive) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player2.angle -= 3.0f; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player2.angle += 3.0f; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { player2.accelerate(0.1f); }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { player2.accelerate(-0.05f); }
                player2.decelerate();
                player2.update();
            }

            // Balas e Asteroides sempre atualizam enquanto o jogo está ativo (não Game Over geral)
            for (auto& bullet : bullets1) bullet.update();
            for (auto& bullet : bullets2) bullet.update();
            for (auto& asteroid : asteroids) asteroid.update();

            // Spawn de novos asteroides (só enquanto o jogo não for Game Over geral)
            if (asteroidClock.getElapsedTime().asSeconds() > 5.0f && asteroids.size() < 10) {
                int side = rand() % 4; sf::Vector2f pos;
                if (side == 0) pos = sf::Vector2f(-50, rand() % (HEIGHT - 100));
                else if (side == 1) pos = sf::Vector2f(WIDTH + 50, rand() % (HEIGHT - 100));
                else if (side == 2) pos = sf::Vector2f(rand() % WIDTH, -50);
                else pos = sf::Vector2f(rand() % WIDTH, HEIGHT + 50);
                float x = rand() % WIDTH; // <--- ADICIONE 'float' AQUI
                float y = rand() % (HEIGHT - 100);
                float vx = (rand() % 100) / 50.0f - 1.0f; 
                float vy = (rand() % 100) / 50.0f - 1.0f;
                asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), 3);
                asteroidClock.restart();
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
                            asteroids.erase(asteroids.begin() + i); break;
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
                            asteroids.erase(asteroids.begin() + i); break;
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