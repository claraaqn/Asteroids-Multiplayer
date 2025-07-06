#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>    
#include <cstdlib>   
#include <ctime>     
#include <SFML/Audio.hpp>
#include <iostream>

#include "GameOverScreen.h"
#include "PerformanceTracker.h"
#include "Menu.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "Game.h"
#include "GameConstants.h"
#include "Starfield.h"
#include "AsteroidExplosion.h"

// para avaliar o código
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace GameConstants; 

int main() {

    srand(static_cast<unsigned int>(time(NULL)));
    sf::Listener::setGlobalVolume(100);

    //? para os asteroides
    sf::Clock gameTimeClock;  // Relógio para medir o tempo total de jogo
    float baseAsteroidSpeed = 50.0f;
    float maxAsteroidSpeed = 300.0f;
    float speedIncreaseRate = 0.4f;  

    float baseSpawnInterval = 1.2f;  
    float minSpawnInterval = 0.6f; // baixa par fircar mais dificil
    float spawnAcceleration = 0.005f; // aumenta pra ficar mais dificil 
    int baseAsteroidsPerSpawn = 1;
    int maxAsteroidsPerSpawn = 4; 

    //? Verifica se há joysticks conectados
    if (sf::Joystick::isConnected(0)) {
        std::cout << "Joystick 0 conectado!" << std::endl;
    }
    if (sf::Joystick::isConnected(1)) {
        std::cout << "Joystick 1 conectado!" << std::endl;
    }

    srand(static_cast<unsigned int>(time(NULL)));
    
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height), "Asteroids Multiplayer", sf::Style::None); 
    
    window.setFramerateLimit(60);
    window.setPosition(sf::Vector2i(0, 0)); 
    window.setMouseCursorVisible(false); 
    window.setVerticalSyncEnabled(false);

    sf::View gameView(sf::FloatRect(0, 0, WIDTH, HEIGHT)); 
    gameView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f)); 
    window.setView(gameView);
    sf::Clock clock;
    sf::Font font;
    if (!font.loadFromFile("assets\\font\\PixelifySans-Regular.ttf")) {
        std::cerr << "Arquivo de fonte não encontrado!" << std::endl;
        return EXIT_FAILURE;
    }
    
    //? --- TELA INICIAL ---
    Menu menu(window, font);
    bool inMenu = true;
    
    //? Tela de Game_Over
    GameOverScreen gameOverScreen(font);
    
    while (inMenu && window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (menu.handleInput(event)) {
                inMenu = false;
            }
        }
        
        menu.draw();
    }
    
    if (!window.isOpen()) {
        return 0; // Sai se o usuário fechou a janela no menu
    }
    window.setView(gameView);

    // --- CARREGAMENTO DE EFEITOS SONOROS ---
    sf::SoundBuffer shootBuffer;
    if (!shootBuffer.loadFromFile("assets/sound/laser1.wav")) {
        std::cerr << "Erro ao carregar som de tiro!" << std::endl;
        return EXIT_FAILURE;
    }

    sf::SoundBuffer explosionBuffer;
    if (!explosionBuffer.loadFromFile("assets/sound/explosion.wav")) {
        std::cerr << "Erro ao carregar som de explosão!" << std::endl;
        return EXIT_FAILURE;
    }
    

    Game game; // Instância do gerenciador de estado do jogo
    Starfield starfield(200, WIDTH, HEIGHT);

    sf::RectangleShape divider(sf::Vector2f(2, HEIGHT));
    divider.setFillColor(sf::Color::White);
    divider.setPosition(WIDTH/2, 0);

    Spaceship player1(sf::Vector2f(WIDTH/4, HEIGHT - 40), 0, true);
    player1.shape.setOutlineColor(sf::Color::Green);

    Spaceship player2(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);
    player2.shape.setOutlineColor(sf::Color::Cyan);

    std::vector<Bullet> bullets1(20);
    std::vector<Bullet> bullets2(20);
    std::vector<sf::Sound> activeSounds;

    for (auto& bullet : bullets1) { bullet.shape.setFillColor(sf::Color::Green); bullet.isActive = false; }
    for (auto& bullet : bullets2) { bullet.shape.setFillColor(sf::Color::Cyan); bullet.isActive = false; }

    sf::Clock explosionClock;
    std::vector<Asteroid> asteroids;
    sf::Clock asteroidClock;
    std::vector<AsteroidExplosion> asteroidExplosions;
    sf::Clock gameClock;

    //! desemhar a animação
    auto drawExplosionAnimation = [&]() {
        for (size_t i = 0; i < asteroidExplosions.size(); ) {
            float progress = asteroidExplosions[i].timer / 0.3f; // Animação de 0.3 segundos
            
            if (progress >= 1.0f) {
                asteroidExplosions.erase(asteroidExplosions.begin() + i);
                continue;
            }
            
            float scale = 1.0f + progress * 2.0f; // Efeito de expansão
            float alpha = 255 * (1.0f - progress); // Fade out
            
            sf::CircleShape explosionShape(asteroidExplosions[i].originalSize * 10 * scale);
            explosionShape.setOrigin(explosionShape.getRadius(), explosionShape.getRadius());
            explosionShape.setPosition(asteroidExplosions[i].position);
            explosionShape.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            explosionShape.setOutlineColor(sf::Color(255, 100, 0, static_cast<sf::Uint8>(alpha)));
            explosionShape.setOutlineThickness(2.0f);
            
            window.draw(explosionShape);
            i++;
        }
    };
    
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

        game.checkGameOver(player1.isAlive, player2.isAlive, score1, score2);
        // --- Renderização ---
        window.clear(sf::Color::Black);  // Limpa a tela uma única vez

        // Limpeza de explosões terminadas
        asteroidExplosions.erase(std::remove_if(asteroidExplosions.begin(), asteroidExplosions.end(),
            [](const AsteroidExplosion& e) { 
                return e.timer >= 0.3f; // Remove após 0.3 segundos
            }), 
            asteroidExplosions.end());

        // Limpeza de balas inativas ou fora da tela
        auto cleanBullets = [](std::vector<Bullet>& bullets) {
        // Apenas desativa as balas, não as remove do vetor
            for (auto& bullet : bullets) {
                if (bullet.isActive && 
                (bullet.shape.getPosition().x < -50 || 
                    bullet.shape.getPosition().x > WIDTH + 50 ||
                    bullet.shape.getPosition().y < -50 ||
                    bullet.shape.getPosition().y > HEIGHT + 50)) {
                    bullet.isActive = false;
                }
            }
        };
        cleanBullets(bullets1);
        cleanBullets(bullets2);

        // Limpeza de sons terminados (já existe no seu código, mantenha)
        activeSounds.erase(std::remove_if(activeSounds.begin(), activeSounds.end(), 
            [](const sf::Sound& s){ return s.getStatus() == sf::Sound::Stopped; }), 
            activeSounds.end());
        
        // Desenha o fundo estrelado primeiro
        starfield.draw(window);

        // Desenha o divisor
        window.draw(divider);
        activeSounds.erase(std::remove_if(activeSounds.begin(), activeSounds.end(), 
            [](const sf::Sound& s){ return s.getStatus() == sf::Sound::Stopped; }), 
                   activeSounds.end());

        static sf::Clock clock; // Declare como static sf::Clock
        static float currentTime = 0.0f; 
        float deltaTime = clock.restart().asSeconds();
        currentTime += deltaTime;

        for (auto& explosion : asteroidExplosions) {
            explosion.timer += deltaTime;
        }

        static float currentTimeAsteroids = 0.0f;
        currentTimeAsteroids += deltaTime;

        for (auto& asteroid : asteroids) {
            asteroid.update(deltaTime, currentTimeAsteroids);
        }


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
            if ((game.isGameOver() && (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)) || 
            (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.button == 7)) {
                if (event.key.code == sf::Keyboard::R) {
                    game.reset(); 

                    player1 = Spaceship(sf::Vector2f(WIDTH/4, HEIGHT - 40), 0, true);
                    player1.shape.setOutlineColor(sf::Color::Green);
                    
                    player2 = Spaceship(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);
                    player2.shape.setOutlineColor(sf::Color::Cyan);
                    
                    bullets1.clear();
                    for (int i = 0; i < 10; ++i) {
                        bullets1.emplace_back();
                        bullets1.back().shape.setFillColor(sf::Color::Green);
                        bullets1.back().isActive = false;
                    }
                    
                    bullets2.clear();
                    for (int i = 0; i < 10; ++i) {
                        bullets2.emplace_back();
                        bullets2.back().shape.setFillColor(sf::Color::Cyan);
                        bullets2.back().isActive = false;
                    }
                    
                    asteroids.clear();
                    for (int i = 0; i < 5; ++i) {
                        float x = rand() % WIDTH; 
                        float y = -50.0f - (rand() % 100); 
                        float vx = (rand() % 100) / 50.0f - 1.0f; 
                        float vy = 30.0f + (rand() % 100) / 20.0f;  // Entre 30.0 e 35.0  
                        int size = (rand() % 2) + 2; //todo Tamanho 2 ou 3
                        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), size);
                    }
                    score1 = 0; score2 = 0; 
            
                } // --- Fim da Lógica de Reinício do Jogo ---
           
            }

            // Sistema de tiro por evento (se a nave estiver viva)
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && player1.canFire() && player1.isAlive) {
                    for (auto& bullet : bullets1) {
                        if (!bullet.isActive) { 
                            bullet.fire(player1.getFirePosition(), player1.angle); 
                            bullet.isActive = true;
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
                                bullet.isActive = true;
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

            //? LÓGICA DE JOGO
            if (!game.isGameOver()) { 

                //! CONTROLES
                //TODO: Controles do jogador 1 (Joystick 0)
                if (player1.isAlive) {
                    // Eixo X do analógico esquerdo para movimento lateral
                    float joystickX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
                    // Eixo Y do analógico esquerdo para movimento frente/trás
                    float joystickY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
                    
                    // Deadzone de 15% - precisa mover o joystick além de 15% de sua amplitude total para que o movimento seja detectado
                    if (std::abs(joystickX) > 25.0f || std::abs(joystickY) > 25.0f) {
                        // Normaliza os valores do joystick
                        float normX = (joystickX / 100.0f) * 0.7f;
                        float normY = (-joystickY / 100.0f) * 0.7f; //* Invertido porque em SFML, Y cresce para baixo
                        
                        // Calcula a direção do movimento baseado no ângulo da nave
                        float radAngle = player1.angle * (3.14159265f / 180.0f); // Converte para radianos
                        
                        // Se você quiser movimento relativo à direção da nave (forward/backward + strafe)
                        float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
                        float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
                        
                        // Aplica as forças
                        // Movimento frontal puro sem influência lateral
                        player1.velocity.x += forwardForce * 0.25f;
                        player1.velocity.y += -forwardForce * 0.25f; 
                        // Movimento lateral puro
                        player1.velocity.x += lateralForce * 0.25f;
                        player1.velocity.y += lateralForce * 0.25f;
                    }

                    // Botão A (0) para atirar
                    if (sf::Joystick::isButtonPressed(0, 0) && player1.canFire()) {
                        for (auto& bullet : bullets1) {
                            if (!bullet.isActive) { 
                                bullet.fire(player1.getFirePosition(), player1.angle); 
                                player1.resetFireCooldown();
                                activeSounds.emplace_back();
                                activeSounds.back().setBuffer(shootBuffer);
                                activeSounds.back().setVolume(70);
                                activeSounds.back().play(); 
                                break; 
                            }
                        }
                    }

                    player1.decelerate();
                    player1.update();
                }

                //TODO: Controles do jogador 2 (Joystick 1) - similar ao jogador 1
                if (player2.isAlive) {
                float joystickX = sf::Joystick::getAxisPosition(1, sf::Joystick::X);
                float joystickY = sf::Joystick::getAxisPosition(1, sf::Joystick::Y);
                
                if (std::abs(joystickX) > 25.0f || std::abs(joystickY) > 25.0f) {
                    float normX = (joystickX / 100.0f) * 0.7f;
                    float normY = (-joystickY / 100.0f) * 0.7f;
                    
                    float radAngle = player2.angle * (3.14159265f / 180.0f);
                    
                    float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
                    float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
                    
                    player2.velocity.x += forwardForce * 0.25f;
                    player2.velocity.y += -forwardForce * 0.25f; 
                    player2.velocity.x += lateralForce * 0.25f;
                    player2.velocity.y += lateralForce * 0.25f;

                }

                if (sf::Joystick::isButtonPressed(1, 0) && player2.canFire()) {
                    for (auto& bullet : bullets2) {
                        if (!bullet.isActive) { 
                            bullet.fire(player2.getFirePosition(), player2.angle); 
                            player2.resetFireCooldown(); 
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(shootBuffer);
                            activeSounds.back().setVolume(70);
                            activeSounds.back().play();
                            break; 
                        }
                    }
                }

                player2.decelerate();
                player2.update();
            }

                // Balas e Asteroides sempre atualizam enquanto o jogo está ativo (não Game Over geral)
                for (auto& bullet : bullets1) bullet.update(deltaTime);
                for (auto& bullet : bullets2) bullet.update(deltaTime);
                for (auto& asteroid : asteroids) asteroid.update(deltaTime, currentTime);

                //! SPAWN DE NOVOS ASTEROIDES
                float currentGameTime = gameTimeClock.getElapsedTime().asSeconds();

                // limite máximo de asteroides
                const int MAX_ASTEROIDS = 100;

                // Calcula o intervalo atual de spawn (diminui com o tempo)
                float currentSpawnInterval = std::max(
                    baseSpawnInterval - (currentGameTime * spawnAcceleration),
                    minSpawnInterval
                );

                // Calcula quantos asteroides spawnar neste momento (aumenta com o tempo)
                int asteroidsToSpawn = std::min(
                    baseAsteroidsPerSpawn + static_cast<int>(currentGameTime / 30),
                    maxAsteroidsPerSpawn
                );

                if (asteroidClock.getElapsedTime().asSeconds() > currentSpawnInterval && asteroids.size() < MAX_ASTEROIDS) {
                    static bool spawnLeft = true;
                    
                    // Calcula a velocidade atual
                    float currentSpeed = std::min(
                        baseAsteroidSpeed + (currentGameTime * speedIncreaseRate),
                        maxAsteroidSpeed
                    );

                    for (int i = 0; i < asteroidsToSpawn; i++) {
                        float x;
                        if (spawnLeft) {
                            x = rand() % (WIDTH / 3);  
                        } else {
                            x = (WIDTH * 2 / 3) + rand() % (WIDTH / 3);  
                        }
                        spawnLeft = !spawnLeft;
                        
                        float y = -50 - (i * 30); // Pequeno deslocamento vertical para múltiplos asteroides
                        float vx = (rand() % 100) / 100.0f - 0.5f;
                        float vy = currentSpeed + (rand() % 30) / 10.0f;
                        
                        int size = (rand() % 2) + 2; // Tamanho 2 ou 3
                        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), size);
                    }
                    
                    asteroidClock.restart();
                }

                //? Colisões - player 1
                if (player1.isAlive) {
                    for (auto& bullet : bullets1) {
                        if (!bullet.isActive) continue;
                        
                        for (size_t i = 0; i < asteroids.size(); ) {
                            sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                            float distanceSquared = diff.x * diff.x + diff.y * diff.y;
                            float radiusSum = asteroids[i].getRadius() + bullet.shape.getRadius();
                            
                            if (distanceSquared < radiusSum * radiusSum) {
                                bullet.isActive = false;
                                score1 += (4 - asteroids[i].getSize()) * 10; // Ajuste para score2 no jogador 2
                                
                                // Efeito de explosão
                                asteroidExplosions.push_back({
                                    asteroids[i].getPosition(),
                                    0.0f,
                                    asteroids[i].getSize()
                                });

                                // Posição onde o asteroide foi atingido
                                sf::Vector2f hitPosition = asteroids[i].getPosition();
                                int originalSize = asteroids[i].getSize();

                                // Remove o asteroide atingido
                                asteroids.erase(asteroids.begin() + i);

                                // Se era tamanho 2 ou 3, cria novos asteroides de tamanho 1 no mesmo lugar
                                if (originalSize > 1) {
                                    int fragments = originalSize; // 2 ou 3 fragmentos
                                    for (int j = 0; j < fragments; j++) {
                                        // Velocidade aleatória para os fragmentos
                                        float angle = (360.0f / fragments) * j + (rand() % 45 - 22.5f); // Dispersão
                                        float speed = 20.0f + (rand() % 100) / 20.0f;  // Entre 4.0 e 9.0
                                        sf::Vector2f velocity(
                                            speed * std::cos(angle * PI / 180.0f),
                                            speed * std::sin(angle * PI / 180.0f)
                                        );

                                        // Cria novo asteroide de tamanho 1 na posição do impacto
                                        asteroids.emplace_back(hitPosition, velocity, 1);
                                    }
                                }

                                // Som de explosão
                                activeSounds.emplace_back();
                                activeSounds.back().setBuffer(explosionBuffer);
                                activeSounds.back().setVolume(70);
                                activeSounds.back().play();

                                break; // Sai do loop após acertar um asteroide
                            } else {
                                i++; // Próximo asteroide se não houve colisão
                            }
                        }
                    }
                    
                    // Colisão nave-asteroide para Player 1
                    for (const auto& asteroid : asteroids) {
                        sf::Vector2f diff = player1.position - asteroid.getPosition();
                        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                        if (distance < asteroid.getRadius() + 20) { 
                            player1.isAlive = false; 
                            break; 
                        }
                    }
                }

                //? Colisões - player 2
                if (player2.isAlive) {
                    for (auto& bullet : bullets2) {
                        if (!bullet.isActive) continue;
                        
                        for (size_t i = 0; i < asteroids.size(); ) {
                            sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                            float distanceSquared = diff.x * diff.x + diff.y * diff.y;
                            float radiusSum = asteroids[i].getRadius() + bullet.shape.getRadius();
                            
                            if (distanceSquared < radiusSum * radiusSum) {
                                bullet.isActive = false;
                                score2 += (4 - asteroids[i].getSize()) * 10; // Ajuste para score2 no jogador 2
                                
                                // Efeito de explosão
                                asteroidExplosions.push_back({
                                    asteroids[i].getPosition(),
                                    0.0f,
                                    asteroids[i].getSize()
                                });

                                // Posição onde o asteroide foi atingido
                                sf::Vector2f hitPosition = asteroids[i].getPosition();
                                int originalSize = asteroids[i].getSize();

                                // Remove o asteroide atingido
                                asteroids.erase(asteroids.begin() + i);

                                // Se era tamanho 2 ou 3, cria novos asteroides de tamanho 1 no mesmo lugar
                                if (originalSize > 1) {
                                    int fragments = originalSize; // 2 ou 3 fragmentos
                                    for (int j = 0; j < fragments; j++) {
                                        // Velocidade aleatória para os fragmentos
                                        float angle = (360.0f / fragments) * j + (rand() % 45 - 22.5f); // Dispersão
                                        float speed = 1.5f + (rand() % 100) / 50.0f;
                                        sf::Vector2f velocity(
                                            speed * std::cos(angle * PI / 180.0f),
                                            speed * std::sin(angle * PI / 180.0f)
                                        );

                                        // Cria novo asteroide de tamanho 1 na posição do impacto
                                        asteroids.emplace_back(hitPosition, velocity, 1);
                                    }
                                }

                                // Som de explosão
                                activeSounds.emplace_back();
                                activeSounds.back().setBuffer(explosionBuffer);
                                activeSounds.back().setVolume(70);
                                activeSounds.back().play();

                                break; // Sai do loop após acertar um asteroide
                            } else {
                                i++; // Próximo asteroide se não houve colisão
                            }
                        }
                    }
                    
                    // Colisão nave-asteroide para Player 2
                    for (const auto& asteroid : asteroids) {
                        sf::Vector2f diff = player2.position - asteroid.getPosition();
                        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                        if (distance < asteroid.getRadius() + 20) { 
                            player2.isAlive = false; 
                            break; 
                        }
                    }
                }
                // --- Fim das Colisões ---

                // A classe Game verifica o estado geral:
                // game.checkGameOver é chamado para ver se QUALQUER um dos players morreu

                scoreText1.setString("P1: " + std::to_string(score1));
                scoreText2.setString("P2: " + std::to_string(score2));

        } // Fim do 'if (!game.isGameOver())' para updates de jogo
    }// Fim do while (window.pollEvent(event))

        starfield.update(deltaTime);
        window.clear(sf::Color::Black);
        starfield.draw(window);
        

        window.draw(divider);

        // Desenha asteroides, balas, naves se estiverem ativas
        drawExplosionAnimation();
        for (const auto& asteroid : asteroids) { 
            asteroid.draw(window); 
        }

        // Naves só são desenhadas se estiverem vivas
        if (player1.isAlive) window.draw(player1.shape);
        if (player2.isAlive) window.draw(player2.shape);

        // Balas sempre são desenhadas se ativas, independentemente do player estar vivo
        for (const auto& bullet : bullets1) { if (bullet.isActive) window.draw(bullet.shape); }
        for (const auto& bullet : bullets2) { if (bullet.isActive) window.draw(bullet.shape); }

        window.draw(scoreText1);
        window.draw(scoreText2);
    
        
        // --- Desenha o texto de reinício GLOBAL se o jogo estiver em GAME_OVER ---
       if (game.isGameOver()) {
    
        static bool updated = false;
    if (!updated) {
        gameOverScreen.update(game.getWinner(), score1, score2);
        gameOverScreen.setPosition(WIDTH/2, HEIGHT/2);
        updated = true;
    }
    
    gameOverScreen.draw(window);
    
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::R) {  // Reiniciar
                game.reset();
                // Reinicialização dos objetos do jogo...
                updated = false;
                break;
            }
            else if (event.key.code == sf::Keyboard::Q) {  // Sair
                window.close();
                return 0;  // Sai imediatamente do jogo
            }
        }
        // Mantenha também o suporte a joystick se desejar
        else if (event.type == sf::Event::JoystickButtonPressed && 
                 event.joystickButton.button == 7) {
            game.reset();
            // Reinicialização...
            updated = false;
            break;
        }
    }
    
    window.display();
    continue;
}
        perfTracker.endFrame();
      
        window.display();
        } // Fim do while (window.isOpen())
    return 0;
}