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

    // Variáveis para tracking de performance
    std::chrono::high_resolution_clock::time_point frameStart;
    sf::Text fpsText;
    sf::Text frameTimeText;
    std::vector<float> frameTimesHistory;  // Para armazenar histórico de tempos de frame

    PerformanceTracker perfTracker;


    srand(static_cast<unsigned int>(time(NULL)));
    sf::Listener::setGlobalVolume(100);

    //? para os asteroides
    sf::Clock gameTimeClock;  // Relógio para medir o tempo total de jogo
    float baseAsteroidSpeed = 50.0f;
    float maxAsteroidSpeed = 300.0f;
    float speedIncreaseRate = 0.4f;  

    float baseSpawnInterval = 1.2f;  
    float minSpawnInterval = 0.6f; // baixa par fircar mais dificil
    float spawnAcceleration = 0.003f; // aumenta pra ficar mais dificil 
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
    //player1.sprite.setOutlineColor(sf::Color::Green);

    Spaceship player2(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);
    //player2.shape.setOutlineColor(sf::Color::Cyan);

    std::vector<Bullet> bullets1(20);
    std::vector<Bullet> bullets2(20);
  
    std::vector<sf::Sound> activeSounds;

    //! Balas
    std::vector<Bullet> bullets1(1);
    std::vector<Bullet> bullets2(1);

    sf::Clock explosionClock;
    std::vector<Asteroid> asteroids;
    sf::Clock asteroidClock;
    std::vector<AsteroidExplosion> asteroidExplosions;
    sf::Clock gameClock;

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

        static sf::Clock clock;
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

        // Atualiza as balas do jogador 1
        for (auto& bullet : bullets1) {
            bullet.update(deltaTime);
        }

        // Atualiza as balas do jogador 2
        for (auto& bullet : bullets2) {
            bullet.update(deltaTime);
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

                    player1.reset(sf::Vector2f(WIDTH/4, HEIGHT - 40), 0, true);
                    player2.reset(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);

                    bullets1.clear();
                    bullets1.resize(1);

                    bullets2.clear();
                    bullets2.resize(1);
                    
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
                // Disparo do player 1 - SPACE (teclado)
                if (event.key.code == sf::Keyboard::Space && player1.canFire() && player1.isAlive) {
                    for (auto& bullet : bullets1) {
                        if (!bullet.isActive) {
                            bullet.fire(player1.getFirePosition(), player1.angle, sf::Color::Green);
                            player1.resetFireCooldown();
                            break;
                        }
                    }
                }
                // Jogador 2 - ENTER (ou use sf::Keyboard::Return)
                if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) && 
                    player2.canFire() && player2.isAlive){               
                        for (auto& bullet : bullets2) {
                            if (!bullet.isActive) {
                                bullet.fire(player2.getFirePosition(), player2.angle, sf::Color::Cyan);
                                player2.resetFireCooldown();
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
                        float rotationSpeed = 2.5f; // Ajuste a sensibilidade aqui
                        player1.angle += joystickX * rotationSpeed * deltaTime; // deltaTime para suavizar
                        
                        // Normaliza o ângulo entre 0-360 graus
                        if (player1.angle > 360) player1.angle -= 360;
                        if (player1.angle < 0) player1.angle += 360;
                        
                        // Atualiza a rotação visual
                        player1.sprite.setRotation(player1.angle);
                        
                        // Normaliza os valores do joystick
                        float normX = (joystickX / 100.0f) * 0.7f;
                        float normY = (-joystickY / 100.0f) * 0.7f; //* Invertido porque em SFML, Y cresce para baixo
                        
                        // Calcula a direção do movimento baseado no ângulo da nave
                        float radAngle = player1.angle * (3.14159265f / 180.0f); // Converte para radianos
                        player1.sprite.setRotation(player1.angle);
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
                                bullet.fire(player1.getFirePosition(), player1.angle, sf::Color::Green); 
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
                        
                    float rotationSpeed = 2.5f; 
                    player2.angle += joystickX * rotationSpeed * deltaTime;
                    
                    // Normaliza ângulo
                    if (player2.angle > 360) player2.angle -= 360;
                    if (player2.angle < 0) player2.angle += 360;
                    
                    player2.sprite.setRotation(player2.angle); 
                    
                    float normX = (joystickX / 100.0f) * 0.7f;
                        float normY = (-joystickY / 100.0f) * 0.7f;
                        
                        float radAngle = player2.angle * (3.14159265f / 180.0f);
                        
                        float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
                        float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
                        player2.sprite.setRotation(player2.angle);

                        player2.velocity.x += forwardForce * 0.25f;
                        player2.velocity.y += -forwardForce * 0.25f; 
                        player2.velocity.x += lateralForce * 0.25f;
                        player2.velocity.y += lateralForce * 0.25f;

                    }

                if (sf::Joystick::isButtonPressed(1, 0) && player2.canFire()) {
                    for (auto& bullet : bullets2) {
                        if (!bullet.isActive) { 
                            bullet.fire(player2.getFirePosition(), player2.angle, sf::Color::Cyan); 
                            player2.resetFireCooldown(); 
                            activeSounds.emplace_back();
                            activeSounds.back().setBuffer(shootBuffer);
                            activeSounds.back().setVolume(70);
                            activeSounds.back().play();
                            break; 
                        }
                    }
                }  
            }

                for (auto& asteroid : asteroids) asteroid.update(deltaTime, currentTime);
                
                //! SPAWN DE NOVOS ASTEROIDES
                float currentGameTime = gameTimeClock.getElapsedTime().asSeconds();

                // Limite máximo de asteroides na tela
                const int MAX_ASTEROIDS = 30;  // Reduzido de 100 para melhor performance

                // Intervalo entre spawns - diminui com o tempo mas tem limite mínimo
                float currentSpawnInterval = std::max(
                    baseSpawnInterval - (currentGameTime * spawnAcceleration),
                    minSpawnInterval
                );

                // Quantidade de asteroides por spawn - aumenta com o tempo mas tem limite máximo
                int asteroidsToSpawn = std::min(
                    baseAsteroidsPerSpawn + static_cast<int>(currentGameTime / 45), // +1 a cada 45 segundos
                    maxAsteroidsPerSpawn
                );

                if (asteroidClock.getElapsedTime().asSeconds() > currentSpawnInterval && 
                    asteroids.size() < MAX_ASTEROIDS) {
                    
                    static bool spawnLeft = true;
                    float currentSpeed = std::min(
                        baseAsteroidSpeed + (currentGameTime * speedIncreaseRate),
                        maxAsteroidSpeed
                    );

                    // Spawn apenas se tiver espaço
                    int canSpawn = std::min(asteroidsToSpawn, MAX_ASTEROIDS - static_cast<int>(asteroids.size()));
                    
                    for (int i = 0; i < canSpawn; i++) {
                        float x = spawnLeft ? 
                            (rand() % (WIDTH / 3)) : 
                            (WIDTH * 2 / 3 + rand() % (WIDTH / 3));
                        
                        float y = -50 - (i * 50); // Espaçamento vertical maior
                        float vx = (rand() % 100) / 100.0f - 0.5f;
                        float vy = currentSpeed * (0.9f + (rand() % 20) / 100.0f); // Variação de 90-110%
                        
                        int size = (rand() % 2) + 2;
                        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), size);
                    }
                    
                    spawnLeft = !spawnLeft;
                    asteroidClock.restart();
                }

                //? Colisões - player 1
                if (player1.isAlive) {
                    for (auto& bullet : bullets1) {
                        
                        for (size_t i = 0; i < asteroids.size(); ) {
                            sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                            float distanceSquared = diff.x * diff.x + diff.y * diff.y;
                            float radiusSum = asteroids[i].getRadius() + bullet.shape.getRadius();
                            
                            if (distanceSquared < radiusSum * radiusSum) {
                                bullet.isActive = false;
                                score1 += (4 - asteroids[i].getSize()) * 10;
                                
                                // Efeito de explosão
                                asteroidExplosions.push_back({
                                    asteroids[i].getPosition(),
                                    0.0f,
                                    asteroids[i].getSize()
                                });

                                sf::Vector2f hitPosition = asteroids[i].getPosition();
                                int originalSize = asteroids[i].getSize();
                                asteroids.erase(asteroids.begin() + i);

                                if (originalSize > 1) {
                                    int fragments = originalSize;
                                    for (int j = 0; j < fragments; j++) {
                                        float angle = (360.0f / fragments) * j + (rand() % 45 - 22.5f);
                                        float speed = 20.0f + (rand() % 100) / 20.0f;
                                        sf::Vector2f velocity(
                                            speed * std::cos(angle * PI / 180.0f),
                                            speed * std::sin(angle * PI / 180.0f)
                                        );
                                        asteroids.emplace_back(hitPosition, velocity, 1);
                                    }
                                }

                                // Som de explosão
                                activeSounds.emplace_back();
                                activeSounds.back().setBuffer(explosionBuffer);
                                activeSounds.back().setVolume(70);
                                activeSounds.back().play();

                                break;
                            } else {
                                i++;
                            }
                        }
                    }
                    
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

                            for (size_t i = 0; i < asteroids.size(); ) {
                                sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                                float distanceSquared = diff.x * diff.x + diff.y * diff.y;
                                float radiusSum = asteroids[i].getRadius() + bullet.shape.getRadius();
                                
                                if (distanceSquared < radiusSum * radiusSum) {
                                    bullet.isActive = false;
                                    score1 += (4 - asteroids[i].getSize()) * 10;
                                    
                                    // Efeito de explosão
                                    asteroidExplosions.push_back({
                                        asteroids[i].getPosition(),
                                        0.0f,
                                        asteroids[i].getSize()
                                    });

                                    sf::Vector2f hitPosition = asteroids[i].getPosition();
                                    int originalSize = asteroids[i].getSize();
                                    asteroids.erase(asteroids.begin() + i);

                                    if (originalSize > 1) {
                                        int fragments = originalSize;
                                        for (int j = 0; j < fragments; j++) {
                                            float angle = (360.0f / fragments) * j + (rand() % 45 - 22.5f);
                                            float speed = 20.0f + (rand() % 100) / 20.0f;
                                            sf::Vector2f velocity(
                                                speed * std::cos(angle * PI / 180.0f),
                                                speed * std::sin(angle * PI / 180.0f)
                                            );
                                            asteroids.emplace_back(hitPosition, velocity, 1);
                                        }
                                    }

                                    // Som de explosão
                                    activeSounds.emplace_back();
                                    activeSounds.back().setBuffer(explosionBuffer);
                                    activeSounds.back().setVolume(70);
                                    activeSounds.back().play();

                                    break;
                                } else {
                                    i++;
                                }
                            }
                        };
                    
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
        window.draw(divider);
    

        // for (const auto& asteroid : asteroids) { 
        //     asteroid.draw(window); 
        // }

        // Desenha asteroides, balas, naves se estiverem ativas
        drawExplosionAnimation();
        for (const auto& asteroid : asteroids) { 
            asteroid.draw(window); 
        }

        // Naves só são desenhadas se estiverem vivas
        if (player1.isAlive) window.draw(player1.sprite);
        if (player2.isAlive) window.draw(player2.sprite);

        for (auto& bullet : bullets1) {
            if (bullet.isActive) {
                window.draw(bullet.shape);
            }
        }

        for (auto& bullet : bullets2) {
            if (bullet.isActive) {
                window.draw(bullet.shape);
            }
        }

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