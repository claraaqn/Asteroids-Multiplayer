// GameSession.cpp
#include "GameSession.h"
#include "GameConstants.h"
#include <iostream>

using namespace GameConstants;

// O construtor inicializa TUDO que era criado no main
GameSession::GameSession(sf::RenderWindow& window, sf::Font& font, GameMode mode)
    : window(window), font(font), gameMode(mode),
      spawnOnLeft(true),
      player1(sf::Vector2f(WIDTH / 4, HEIGHT - 40), 0, true),
      player2(sf::Vector2f(3 * WIDTH / 4, HEIGHT - 40), 0, false),
      starfield(200, WIDTH, HEIGHT),
      score1(0), score2(0), gameTime(0.0f), gameOverScreen(font)
{
    // Carrega sons
    if (!shootBuffer.loadFromFile("assets/sound/laser1.wav")) exit(1);
    if (!explosionBuffer.loadFromFile("assets/sound/explosion.wav")) exit(1);
    
    // Configura o divisor (só para modo multiplayer)
    divider.setSize(sf::Vector2f(2, HEIGHT));
    divider.setFillColor(sf::Color::White);
    divider.setPosition(WIDTH / 2.0f, 0);

    // Configura textos de score
    scoreText1.setFont(font);
    scoreText1.setCharacterSize(20);
    scoreText1.setFillColor(sf::Color::Green);
    scoreText1.setPosition(10, 10);

    scoreText2.setFont(font);
    scoreText2.setCharacterSize(20);
    scoreText2.setFillColor(sf::Color::Cyan);
    scoreText2.setPosition(WIDTH - 100, 10);
    
    // Se for Single Player, "desativa" o jogador 2
    if (gameMode == GameMode::SinglePlayer) {
        player2.isAlive = false;
    }

    resetGame();
}

// GameSession.cpp
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

void GameSession::handleEvents() {
    sf::Event event; // APENAS UMA DECLARAÇÃO
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
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
    if (!player1.isAlive && (gameMode == GameMode::SinglePlayer || !player2.isAlive)) {
        gameState.checkGameOver(player1.isAlive, player2.isAlive, score1, score2);
        if(gameState.isGameOver()){
            gameOverScreen.update(gameState.getWinner(), score1, score2);
            gameOverScreen.setPosition(WIDTH/2, HEIGHT/2);
        }
    }

    // Atualiza textos
    scoreText1.setString("P1: " + std::to_string(score1));
    scoreText2.setString("P2: " + std::to_string(score2));
}

void GameSession::render() {
    window.clear(sf::Color::Black);
    
    starfield.draw(window);
    if (gameMode == GameMode::Multiplayer) {
        window.draw(divider);
    }

    for (const auto& asteroid : asteroids) asteroid.draw(window);
    for (const auto& bullet : bullets1) if (bullet.isActive) window.draw(bullet.shape);
    
    if (player1.isAlive) window.draw(player1.sprite);
    if (player2.isAlive && gameMode == GameMode::Multiplayer) window.draw(player2.sprite);
    
    // Desenhar explosões (sua lógica lambda pode vir para cá)
    for(auto& explosion : asteroidExplosions) { 
        float progress = explosion.timer / 0.3f;
        if (progress >= 1.0f) continue;

        // Anima o tamanho e a transparência
        float scale = 1.0f + progress * 2.0f;
        float alpha = 255.0f * (1.0f - progress);

        sf::CircleShape explosionShape(explosion.originalSize * 10.0f * scale);
        explosionShape.setOrigin(explosionShape.getRadius(), explosionShape.getRadius());
        explosionShape.setPosition(explosion.position);
        explosionShape.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));

        window.draw(explosionShape);
     }

    window.draw(scoreText1);
    if(gameMode == GameMode::Multiplayer) window.draw(scoreText2);
    
    window.display();
}

void GameSession::resetGame() {
    gameState.reset();
    score1 = 0;
    score2 = 0;
    gameTime = 0.0f;
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
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            keyboardY = 1.0f;
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && player1.canFire()) {
            bullets1.emplace_back().fire(player1.getFirePosition(), player1.angle);
            player1.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        }
        // TIRO COM JOYSTICK
        if (sf::Joystick::isButtonPressed(0, 0) && player1.canFire()) {
            bullets1.emplace_back().fire(player1.getFirePosition(), player1.angle);
            player1.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        }

        player1.decelerate();
        player1.update();
    }
}

void GameSession::spawnAsteroids(float deltaTime) {
    const float BASE_SPAWN_INTERVAL   = 1.5f;
    const float MIN_SPAWN_INTERVAL    = 0.3f;
    const float SPAWN_ACCELERATION    = 0.003f;
    const int   BASE_ASTEROIDS_SPAWN  = 1;
    const int   MAX_ASTEROIDS_SPAWN   = 4;
    const float BASE_ASTEROID_SPEED   = 50.0f;
    const float MAX_ASTEROID_SPEED    = 300.0f;
    const float SPEED_INCREASE_RATE   = 0.3f;

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
        for (int i = 0; i < asteroidsToSpawn; i++) {
            float x, y, vx, vy;
            
            if (gameMode == GameMode::Multiplayer) {
                // Multiplayer
                x = spawnOnLeft 
                    ? (rand() % (WIDTH / 3)) 
                    : (WIDTH * 2 / 3 + rand() % (WIDTH / 3));
                vx = (rand() % 100) / 100.0f - 0.5f;
            } else {
                // Singleplayer
                x = rand() % WIDTH;
                
                if (x < WIDTH / 2) {
                    // Se nasceu na metade esquerda, move para a direita
                    vx = (rand() % 100) / 100.0f; // 0.0 a 1.0
                } else {
                    // Se nasceu na metade direita, move para a esquerda
                    vx = -((rand() % 100) / 100.0f); // -1.0 a 0.0
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

// GameSession.cpp

void GameSession::checkCollisions() {
    // Usamos um laço 'for' com índice porque vamos modificar o vetor 'asteroids'
    for (size_t i = 0; i < asteroids.size(); ++i) {
        
        // --- 1. Colisão Asteroide vs. Naves ---
        if (player1.isAlive && player1.getBounds().intersects(asteroids[i].getBounds())) {
            player1.isAlive = false; // "Mata" o jogador 1
            // TODO: Tocar som de morte do jogador
        }

        // --- 2. Colisão Asteroide vs. Balas ---
        // Balas do Jogador 1
        for (auto& bullet : bullets1) {
            if (bullet.isActive && bullet.getBounds().intersects(asteroids[i].getBounds())) {
                bullet.isActive = false;
                destroyAsteroid(i, score1); // Chama nossa função auxiliar!
                goto next_asteroid; // Pula para o próximo asteroide, pois este foi destruído
            }
        }
    }
    next_asteroid:; // Rótulo para o goto
}

void GameSession::updateGameObjects(float deltaTime) {
    starfield.update(deltaTime);
    if (player1.isAlive) player1.update();

    for (auto& bullet : bullets1) bullet.update(deltaTime);
    for (auto& asteroid : asteroids) asteroid.update(deltaTime, gameTime);
    for (auto& explosion : asteroidExplosions) explosion.timer += deltaTime;

    // Limpeza de objetos inativos (balas, explosões)
    bullets1.erase(std::remove_if(bullets1.begin(), bullets1.end(), [](const Bullet& b){ return !b.isActive; }), bullets1.end());
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