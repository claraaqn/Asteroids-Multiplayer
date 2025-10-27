// GameSession.cpp
#include "GameSession.h"
#include "GameConstants.h"
#include <iostream>

using namespace GameConstants;


// O construtor inicializa TUDO que era criado no main
GameSession::GameSession(sf::RenderWindow& window, sf::Font& font, GameMode mode, const sf::View& gameView, const sf::View& hudView)
    : window(window), 
      font(font), 
      gameMode(mode), 
      gameView(gameView), 
      hudView(hudView), // hudView primeiro
      nameInputScreen(window, font),
      nameEntered(false),
      currentPlayerName(""),
      highScoreDB(),
      spawnOnLeft(true),
      player1(sf::Vector2f(WIDTH / 4, HEIGHT - 40), 0, true),
      player2(sf::Vector2f(3 * WIDTH / 4, HEIGHT - 40), 0, false),
      starfield(200, WIDTH, HEIGHT),
      score1(0), score2(0), gameTime(0.0f),
      gameOverScreen(font, highScoreDB)
{

    highScoreDB.initialize();
    // Carrega sons
    if (!shootBuffer.loadFromFile("assets/sound/laser1.wav")) exit(1);
    if (!explosionBuffer.loadFromFile("assets/sound/explosion.wav")) exit(1);

    // Configura textos de score
    if (gameMode == GameMode::SinglePlayer) {
        //! desativa o player 2
        player2.isAlive = false;

        //! pontos
        scoreText1.setFont(font);
        scoreText1.setCharacterSize(30);
        scoreText1.setFillColor(sf::Color::Green);
        scoreText1.setPosition(10, 10);

        //! Configura textos de supertiro
        superShotText1.setFont(font);
        superShotText1.setCharacterSize(20);
        superShotText1.setFillColor(sf::Color::Yellow);
        superShotText1.setPosition(10, 40);
    } else if (gameMode == GameMode::Multiplayer) {
        scoreText1.setFont(font);
        scoreText1.setCharacterSize(30);
        scoreText1.setFillColor(sf::Color::Green);
        scoreText1.setPosition(10, 10);

        scoreText2.setFont(font);
        scoreText2.setCharacterSize(30);
        scoreText2.setFillColor(sf::Color::Cyan);
        scoreText2.setPosition(WIDTH - 100, 10);

        //! Configura textos de supertiro
        superShotText1.setFont(font);
        superShotText1.setCharacterSize(20);
        superShotText1.setFillColor(sf::Color::Yellow);
        superShotText1.setPosition(10, 40);

        superShotText2.setFont(font);
        superShotText2.setCharacterSize(20);
        superShotText2.setFillColor(sf::Color::Yellow);
        superShotText2.setPosition(WIDTH - 100, 40);

        //! configurações de divisão de tela
        divider.setSize(sf::Vector2f(2, HEIGHT));
        divider.setFillColor(sf::Color::White);
        divider.setPosition(WIDTH / 2.0f, 0);
    }

    resetGame();
}

void GameSession::setPlayerName(const std::string& p1Name, const std::string& p2Name) {
    player1Name = p1Name;
    player2Name = p2Name;
    nameEntered = true;
}

void GameSession::run() {
    sf::Clock clock;

    
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        handleEvents();

        if (gameState.isGameOver()) {
            gameOverScreen.draw(window);
            window.display();
            continue;
        }
        
        update(deltaTime);
        render();
    }
}

void GameSession::gameOver(int finalScore) {
    if (gameMode == GameMode::SinglePlayer) {
        highScoreDB.addHighScore(player1Name, score1, gameMode);
    } else {
        highScoreDB.addHighScore(player1Name + " (P1)", score1, gameMode);
        highScoreDB.addHighScore(player2Name + " (P2)", score2, gameMode);
    }
    
    gameOverScreen.refreshHighScores(gameMode);
}

void GameSession::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        // Se estiver na tela de entrada de nome, processa eventos lá
        if (nameInputScreen.isActive()) {
            nameInputScreen.handleEvent(event);
            continue;
        }
        
        if (gameState.isGameOver()) {
            if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) ||
                (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.button == 7)) {
                resetGame();
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                window.close();
            }
            continue;
        }
    }
}

void GameSession::update(float deltaTime) {
    gameTime += deltaTime;
    
    processPlayerInput(deltaTime);
    updateGameObjects(deltaTime);
    spawnAsteroids(deltaTime);
    checkCollisions();

    // VERIFIQUE SE O JOGO DEVE TERMINAR APENAS SE OS JOGADORES ESTIVEREM MORTOs
    // Multiplayer: termina quando qualquer um morrer
    if (gameMode == GameMode::Multiplayer) {
        if (!player1.isAlive || !player2.isAlive) {
            gameState.checkGameOver(player1.isAlive, player2.isAlive, score1, score2);
            if (gameState.isGameOver()) {
                gameOverScreen.update(gameState.getWinner(), score1, score2, gameMode);
                gameOverScreen.setPosition(WIDTH/2, HEIGHT/2);
                int finalScore = (gameState.getWinner() == 1) ? score1 : score2;
                gameOver(finalScore);
            }
        }
    } else { 
        // Singleplayer
        if (!player1.isAlive) {
            gameState.checkGameOver(player1.isAlive, false, score1, score2);
            if (gameState.isGameOver()) {
                gameOverScreen.update(gameState.getWinner(), score1, score2, gameMode);
                gameOverScreen.setPosition(WIDTH/2, HEIGHT/2);
                gameOver(score1);
            }
        }
    }


    //! Atualiza supertiros disponíveis
    superShot1.addShotsBasedOnScore(score1);
    superShot2.addShotsBasedOnScore(score2);

    //! Atualiza textos
    superShotText1.setString("Super: " + std::to_string(superShot1.getAvailableShots()));
    superShotText2.setString("Super: " + std::to_string(superShot2.getAvailableShots()));

    // Atualiza textos
    scoreText1.setString("P1: " + std::to_string(score1));
    scoreText2.setString("P2: " + std::to_string(score2));
}

void GameSession::render() {
    window.clear(sf::Color::Black);
     renderGame(); // Desenha o jogo
    renderHud();  // D
    window.draw(scoreText1);

    
    window.display();
}

void GameSession::resetGame() {
    gameState.reset();
    score1 = 0;
    score2 = 0;
    gameTime = 0.0f;
    superShot1.reset(); 
    superShot2.reset();
    gameClock.restart();

    if (gameMode == GameMode::Multiplayer) {
        player1.reset(sf::Vector2f(WIDTH / 4, HEIGHT - 40), 0, true);
        player2.reset(sf::Vector2f(3 * WIDTH / 4, HEIGHT - 40), 0, false);
        player2.isAlive = true;
    } else {
        // Singleplayer: centraliza o jogador 1
        player1.reset(sf::Vector2f(WIDTH / 2, HEIGHT - 40), 0, true);
        player2.isAlive = false; // Garante que o jogador 2 está inativo
    }

    asteroids.clear();
    bullets1.clear();
    bullets2.clear(); 
    asteroidExplosions.clear();
}

void GameSession::processPlayerInput(float deltaTime) {
    //! --- Controles do Jogador 1 ---
    if (player1.isAlive) {
        //? CONTROLES DE TECLADO (SETINHAS) - ROTAÇÃO E ACELERAÇÃO
        float keyboardX = 0.0f;
        float keyboardY = 0.0f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            keyboardX = -1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            keyboardX = 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            keyboardY = -1.0f;
            player1.setAccelerating(true);
        }
       if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            keyboardY = 1.0f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                player1.setAccelerating(true);

        } else {
            player1.setAccelerating(false);
        }
        
        // Se estiver usando teclado, prioriza sobre joystick
        if (keyboardX != 0.0f || keyboardY != 0.0f) {
            float normX = keyboardX * 0.4f;
            float normY = keyboardY * 0.4f;
            
            // Calcula a direção do movimento baseado no ângulo atual da nave
            float radAngle = player1.angle * (3.14159265f / 180.0f);
            
            // Movimento relativo à direção da nave (forward/backward + strafe)
            float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
            float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
            
            // Aplica as forças
            player1.velocity.x += forwardForce * 0.25f;
            player1.velocity.y += -forwardForce * 0.25f; 
            player1.velocity.x += lateralForce * 0.25f;
            player1.velocity.y += lateralForce * 0.25f;
        }
        else {
            //? CONTROLES DE JOYSTICK - MOVIMENTO DIRECIONAL
            float joystickX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
            float joystickY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
            
            if (std::abs(joystickX) > 25.0f || std::abs(joystickY) > 25.0f) {
                // Para joystick, não rotaciona automaticamente - usa movimento direcional
                float normX = (joystickX / 100.0f) * 0.4f;
                float normY = (-joystickY / 100.0f) * 0.4f;
                
                // Calcula a direção do movimento baseado no ângulo atual da nave
                float radAngle = player1.angle * (3.14159265f / 180.0f);
                
                // Movimento relativo à direção da nave (forward/backward + strafe)
                float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
                float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
                
                // Aplica as forças
                player1.velocity.x += forwardForce * 0.25f;
                player1.velocity.y += -forwardForce * 0.25f; 
                player1.velocity.x += lateralForce * 0.25f;
                player1.velocity.y += lateralForce * 0.25f;
            }
        }

        // TIRO COM TECLADO (Barra de Espaço)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && player1.canFire()) {
            bullets1.emplace_back().fire(player1.getFirePosition(), player1.angle);
            player1.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        }
        //! SUPERTIRO COM TECLADO (Shift)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
            superShot1.fire(player1, bullets1, score1);
        }
        // TIRO COM JOYSTICK
        if (sf::Joystick::isButtonPressed(0, 0) && player1.canFire()) {
            bullets1.emplace_back().fire(player1.getFirePosition(), player1.angle);
            player1.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        }
        //! SUPERTIRO COM JOYSTICK (B)
        if (sf::Joystick::isButtonPressed(0, 1)) {
            superShot1.fire(player1, bullets1, score1);
        }

        player1.decelerate();
        player1.update(deltaTime, gameMode == GameMode::SinglePlayer);
    }

    //! --- Controles do Jogador 2 (apenas no multiplayer) ---
    if (gameMode == GameMode::Multiplayer && player2.isAlive) {
        //? CONTROLES DE TECLADO (WASD) - ROTAÇÃO E ACELERAÇÃO
        float keyboardX = 0.0f;
        float keyboardY = 0.0f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            keyboardX = -1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            keyboardX = 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            keyboardY = -1.0f;
            player2.setAccelerating(true);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            keyboardY = 1.0f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            player2.setAccelerating(true);
        } else {
            player2.setAccelerating(false);
        }
        
        // Se estiver usando teclado, prioriza sobre joystick
        if (keyboardX != 0.0f || keyboardY != 0.0f) {
            float normX = keyboardX * 0.4f;
            float normY = keyboardY * 0.4f;
            
            float radAngle = player2.angle * (3.14159265f / 180.0f);
            
            float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
            float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
            
            player2.velocity.x += forwardForce * 0.25f;
            player2.velocity.y += -forwardForce * 0.25f; 
            player2.velocity.x += lateralForce * 0.25f;
            player2.velocity.y += lateralForce * 0.25f;
        }
        else {
            //? CONTROLES DE JOYSTICK - MOVIMENTO DIRECIONAL (Jogador 2 - Joystick 1)
            float joystickX = sf::Joystick::getAxisPosition(1, sf::Joystick::X);
            float joystickY = sf::Joystick::getAxisPosition(1, sf::Joystick::Y);
            
            if (std::abs(joystickX) > 25.0f || std::abs(joystickY) > 25.0f) {
                float normX = (joystickX / 100.0f) * 0.4f;
                float normY = (-joystickY / 100.0f) * 0.4f;
                
                float radAngle = player2.angle * (3.14159265f / 180.0f);
                
                float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
                float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
                
                player2.velocity.x += forwardForce * 0.25f;
                player2.velocity.y += -forwardForce * 0.25f; 
                player2.velocity.x += lateralForce * 0.25f;
                player2.velocity.y += lateralForce * 0.25f;
            }
        }

        // TIRO COM TECLADO (Jogador 2 - Barra de Espaço)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && player2.canFire()) {
            bullets2.emplace_back().fire(player2.getFirePosition(), player2.angle);
            player2.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        }
        //! SUPERTIRO COM TECLADO (Jogador 2 - Ctrl)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
            superShot2.fire(player2, bullets2, score2);
        }
        // TIRO COM JOYSTICK (Jogador 2 - Botão A)
        if (sf::Joystick::isButtonPressed(1, 0) && player2.canFire()) {
            bullets2.emplace_back().fire(player2.getFirePosition(), player2.angle);
            player2.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        }
        //! SUPERTIRO COM JOYSTICK (Jogador 2 - Botão B)
        if (sf::Joystick::isButtonPressed(1, 1)) {
            superShot2.fire(player2, bullets2, score2);
        }

        player2.decelerate();
        player2.update(deltaTime, false);
    }
}

void GameSession::spawnAsteroids(float deltaTime) {
    const float BASE_SPAWN_INTERVAL   = 1.5f; //! talvez possa diminuir
    const float MIN_SPAWN_INTERVAL    = 0.3f; //! talvez possa deminuir
    const float SPAWN_ACCELERATION    = 0.003f; //! talvez possa aumentar
    const int   BASE_ASTEROIDS_SPAWN  = 1;
    const int   MAX_ASTEROIDS_SPAWN   = 4; //! talvez possa aumentar
    const float BASE_ASTEROID_SPEED   = 50.0f;
    const float MAX_ASTEROID_SPEED    = 300.0f;
    const float SPEED_INCREASE_RATE   = 0.3f; //! talvez possa almentar

    // Calcula o intervalo de spawn
    float currentSpawnInterval = std::max(
        BASE_SPAWN_INTERVAL - (gameTime * SPAWN_ACCELERATION),
        MIN_SPAWN_INTERVAL
    );

    if (asteroidSpawnClock.getElapsedTime().asSeconds() > currentSpawnInterval) {
        
        // Calcula quantos asteroides criar
        int asteroidsToSpawn = std::min(
            BASE_ASTEROIDS_SPAWN + static_cast<int>(gameTime / 60),
            MAX_ASTEROIDS_SPAWN
        );

        // Calcula a velocidade
        float currentSpeed = std::min(
            BASE_ASTEROID_SPEED + (gameTime * SPEED_INCREASE_RATE),
            MAX_ASTEROID_SPEED
        );

        // Spawna todos os asteroides calculados, independente de quantos já existem
        //TODO: talvez precide melhorar
        for (int i = 0; i < asteroidsToSpawn; i++) {
            float x, y, vx, vy;
            
            if (gameMode == GameMode::Multiplayer) {
                // Multiplayer
                if (spawnOnLeft) {
                    x = rand() % WIDTH; 
                    vx = -((rand() % 70) / 100.0f + 0.3f); 
                } else {
                    x = rand() % WIDTH;
                    vx = (rand() % 70) / 100.0f + 0.3f; 
                }
            } else {
                // Singleplayer 
                x = rand() % WIDTH;  
                
                float randomDirection = (rand() % 100) / 100.0f; 
                
                if (randomDirection < 0.4f) {
                    // 40% chance: movimento suave para o centro
                    if (x < WIDTH / 2) {
                        vx = (rand() % 60) / 100.0f + 0.2f; 
                    } else {
                        vx = -((rand() % 60) / 100.0f + 0.2f); 
                    }
                } else if (randomDirection < 0.7f) {
                    // 30% chance: movimento quase vertical
                    vx = (rand() % 40) / 100.0f - 0.2f; 
                } else {
                    // 30% chance: movimento diagonal acentuado
                    if (x < WIDTH / 2) {
                        vx = (rand() % 80) / 100.0f + 0.5f; 
                    } else {
                        vx = -((rand() % 80) / 100.0f + 0.5f); 
                }
            }
            
            
            y = -50.0f - (i * 30.0f);
            vy = currentSpeed * (0.8f + (rand() % 40) / 100.0f);
            
            int size = (rand() % 2) + 2; // Tamanho 2 ou 3
            
            asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), size);
        
        }
        
        if (gameMode == GameMode::Multiplayer) {
            spawnOnLeft = !spawnOnLeft;
        }
        
        asteroidSpawnClock.restart();
    }
}
}

void GameSession::checkCollisions() {
    for (size_t i = 0; i < asteroids.size(); ++i) {
        
        // --- 1. Colisão Asteroide vs. Naves (usando círculos) ---
        if (player1.isAlive) {
            sf::Vector2f playerPos = player1.sprite.getPosition();
            sf::Vector2f asteroidPos = asteroids[i].getPosition();
            
            float distance = std::sqrt(
                std::pow(playerPos.x - asteroidPos.x, 2) + 
                std::pow(playerPos.y - asteroidPos.y, 2)
            );
            
            float collisionDistance = player1.getCollisionRadius() + asteroids[i].getCollisionRadius();
            
            if (distance < collisionDistance) {
                player1.isAlive = false;
            }
        }
        
        if (gameMode == GameMode::Multiplayer && player2.isAlive) {
            sf::Vector2f playerPos = player2.sprite.getPosition();
            sf::Vector2f asteroidPos = asteroids[i].getPosition();
            
            float distance = std::sqrt(
                std::pow(playerPos.x - asteroidPos.x, 2) + 
                std::pow(playerPos.y - asteroidPos.y, 2)
            );
            
            float collisionDistance = player2.getCollisionRadius() + asteroids[i].getCollisionRadius();
            
            if (distance < collisionDistance) {
                player2.isAlive = false;
            }
        }

        // --- 2. Colisão Asteroide vs. Balas (mantém retângulo para ser mais permissivo) ---
        // Balas do Jogador 1
        for (auto& bullet : bullets1) {
            if (bullet.isActive && bullet.getBounds().intersects(asteroids[i].getBounds())) {
                bullet.isActive = false;
                destroyAsteroid(i, score1);
                goto next_asteroid;
            }
        }
        
        // Balas do Jogador 2
        for (auto& bullet : bullets2) {
            if (bullet.isActive && bullet.getBounds().intersects(asteroids[i].getBounds())) {
                bullet.isActive = false;
                destroyAsteroid(i, score2);
                goto next_asteroid;
            }
        }
    }
    next_asteroid:;
}

void GameSession::updateGameObjects(float deltaTime) {
    starfield.update(deltaTime);
    if (player1.isAlive) player1.update(deltaTime, gameMode == GameMode::SinglePlayer);
    if (player2.isAlive && gameMode == GameMode::Multiplayer) player2.update(deltaTime, false);

    for (auto& bullet : bullets1) bullet.update(deltaTime);
    for (auto& bullet : bullets2) bullet.update(deltaTime);
    for (auto& asteroid : asteroids) asteroid.update(deltaTime, gameTime);
    for (auto& explosion : asteroidExplosions) explosion.timer += deltaTime;

    bullets1.erase(std::remove_if(bullets1.begin(), bullets1.end(), [](const Bullet& b){ return !b.isActive; }), bullets1.end());
    bullets2.erase(std::remove_if(bullets2.begin(), bullets2.end(), [](const Bullet& b){ return !b.isActive; }), bullets2.end());
    asteroidExplosions.erase(std::remove_if(asteroidExplosions.begin(), asteroidExplosions.end(), [](const AsteroidExplosion& e){ return e.timer >= 0.3f; }), asteroidExplosions.end());
}

void GameSession::destroyAsteroid(size_t index, int& playerScore) {
    // Guarda a posição e o tamanho antes de remover o asteroide
    sf::Vector2f position = asteroids[index].getPosition();
    int size = asteroids[index].getSize();

    // 1. Adiciona pontos ao jogador correto
    playerScore += (4 - size) * 10;

    // 2. Adiciona um efeito visual de explosão
    asteroidExplosions.push_back({position, 0.0f, size});

    // 3. Toca o som de explosão
    activeSounds.emplace_back(explosionBuffer);
    activeSounds.back().setVolume(70);
    activeSounds.back().play();

    // 4. Remove o asteroide da lista
    asteroids.erase(asteroids.begin() + index);

    // 5. Se não era um asteroide pequeno, cria fragmentos menores
    if (size > 1) {
        for (int i = 0; i < 2; ++i) { // Cria 2 fragmentos
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            sf::Vector2f velocity(std::cos(angle) * 75.f, std::sin(angle) * 75.f);
            asteroids.emplace_back(position, velocity, size - 1);
        }
    }
}

void GameSession::renderGame() {
    window.setView(gameView);

    starfield.draw(window);
    if (gameMode == GameMode::Multiplayer) {
        window.draw(divider);
    }
    for (const auto& asteroid : asteroids) asteroid.draw(window);
    
    // Balas do jogador 1
    for (const auto& bullet : bullets1) if (bullet.isActive) window.draw(bullet.shape);
    
    // Balas do jogador 2
    for (const auto& bullet : bullets2) if (bullet.isActive) window.draw(bullet.shape);

    // Jogador 1
    if (player1.isAlive) {
        window.draw(player1.sprite);
        player1.draw(window);
    }

    // Jogador 2 (apenas no multiplayer)
    if (gameMode == GameMode::Multiplayer && player2.isAlive) {
        window.draw(player2.sprite);
        player2.draw(window);
    }
}

void GameSession::renderHud() {
    window.setView(hudView); 

    window.draw(scoreText1);
    window.draw(scoreText2);
    window.draw(superShotText1);
    window.draw(superShotText2);

    // Se o jogo acabou, a tela de GameOver também é parte da HUD
    if (gameState.isGameOver()) {
        gameOverScreen.draw(window);
    }
}