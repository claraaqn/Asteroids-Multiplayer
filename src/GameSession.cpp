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

        // CORRIJA AS CHAMADAS DE fire() - remova o terceiro argumento
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space && player1.canFire() && player1.isAlive) {
                bullets1.emplace_back().fire(player1.getFirePosition(), player1.angle); // 2 ARGUMENTOS
                player1.resetFireCooldown();
            }
            if ((event.key.code == sf::Keyboard::Enter) && player2.canFire() && player2.isAlive && gameMode == GameMode::Multiplayer) {
                bullets2.emplace_back().fire(player2.getFirePosition(), player2.angle); // 2 ARGUMENTOS
                player2.resetFireCooldown();
            }
        }
    }
}

void GameSession::update(float deltaTime) {
    gameTime += deltaTime;
    
    processPlayerInput(deltaTime);
    updateGameObjects(deltaTime);
    spawnAsteroids(deltaTime);
    checkCollisions();

    // Atualiza o estado do jogo (verifica se alguém morreu)
    gameState.checkGameOver(player1.isAlive, player2.isAlive, score1, score2);
    if(gameState.isGameOver()){
        gameOverScreen.update(gameState.getWinner(), score1, score2);
        gameOverScreen.setPosition(WIDTH/2, HEIGHT/2);
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
    for (const auto& bullet : bullets2) if (bullet.isActive) window.draw(bullet.shape);
    
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

    player1.reset(sf::Vector2f(WIDTH / 4, HEIGHT - 40), 0, true);
    if (gameMode == GameMode::Multiplayer) {
        player2.reset(sf::Vector2f(3 * WIDTH / 4, HEIGHT - 40), 0, false);
    } else {
        player1.reset(sf::Vector2f(WIDTH / 2, HEIGHT - 40), 0, true); // Centraliza no single player
        player2.isAlive = false;
    }

    asteroids.clear();
    bullets1.clear();
    bullets2.clear();
    asteroidExplosions.clear();
}


// Todas as outras lógicas (input, spawn, colisões) são movidas para cá
// como métodos privados. Por exemplo:

void GameSession::processPlayerInput(float deltaTime) {
    // --- Controles do Jogador 1 ---
    if (player1.isAlive) {
        float joystickX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        float joystickY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
        
        if (std::abs(joystickX) > 25.0f || std::abs(joystickY) > 25.0f) {
            player1.angle += joystickX * 2.5f * deltaTime;
            if (player1.angle > 360) player1.angle -= 360;
            if (player1.angle < 0) player1.angle += 360;
            player1.sprite.setRotation(player1.angle);
            
            float normX = (joystickX / 100.0f) * 0.7f;
            float normY = (-joystickY / 100.0f) * 0.7f;
            
            float radAngle = player1.angle * (3.14159265f / 180.0f);
            float forwardForce = normY * cos(radAngle) - normX * sin(radAngle);
            float lateralForce = normY * sin(radAngle) + normX * cos(radAngle);
            
            player1.velocity.x += forwardForce * 0.25f;
            player1.velocity.y += -forwardForce * 0.25f; 
            player1.velocity.x += lateralForce * 0.25f;
            player1.velocity.y += lateralForce * 0.25f;
        }

        if (sf::Joystick::isButtonPressed(0, 0) && player1.canFire()) {
            // **SUGESTÃO APLICADA**: Adiciona uma nova bala diretamente
            bullets1.emplace_back().fire(player1.getFirePosition(), player1.angle); // 2 ARGUMENTOS
            player1.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play(); 
        }

        player1.decelerate();
        player1.update();
    }

    // --- Controles do Jogador 2 ---
    // **CORREÇÃO CRÍTICA APLICADA**: Adicionado "&& gameMode == GameMode::Multiplayer"
    if (player2.isAlive && gameMode == GameMode::Multiplayer) {
        float joystickX = sf::Joystick::getAxisPosition(1, sf::Joystick::X);
        float joystickY = sf::Joystick::getAxisPosition(1, sf::Joystick::Y);
        
        if (std::abs(joystickX) > 25.0f || std::abs(joystickY) > 25.0f) {
            player2.angle += joystickX * 2.5f * deltaTime;
            if (player2.angle > 360) player2.angle -= 360;
            if (player2.angle < 0) player2.angle += 360;
            player2.sprite.setRotation(player2.angle); 
            
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
            // **SUGESTÃO APLICADA**: Adiciona uma nova bala diretamente
            bullets2.emplace_back().fire(player2.getFirePosition(), player2.angle); // 2 ARGUMENTOS
            player2.resetFireCooldown();
            
            activeSounds.emplace_back(shootBuffer);
            activeSounds.back().setVolume(70);
            activeSounds.back().play();
        } 
        
        player2.decelerate();
        player2.update();
    }
}

void GameSession::spawnAsteroids(float deltaTime) {
    // Parâmetros de dificuldade (agora definidos aqui dentro)
    const int   MAX_ASTEROIDS         = 40;
    const float BASE_SPAWN_INTERVAL   = 1.5f;
    const float MIN_SPAWN_INTERVAL    = 0.5f;
    const float SPAWN_ACCELERATION    = 0.005f;
    const int   BASE_ASTEROIDS_SPAWN  = 1;
    const int   MAX_ASTEROIDS_SPAWN   = 4;
    const float BASE_ASTEROID_SPEED   = 50.0f;
    const float MAX_ASTEROID_SPEED    = 300.0f;
    const float SPEED_INCREASE_RATE   = 0.4f;

    // Calcula o intervalo de spawn, que diminui com o tempo de jogo (gameTime)
    float currentSpawnInterval = std::max(
        BASE_SPAWN_INTERVAL - (gameTime * SPAWN_ACCELERATION),
        MIN_SPAWN_INTERVAL
    );

    // Se passou tempo suficiente e a tela não está cheia...
    if (asteroidSpawnClock.getElapsedTime().asSeconds() > currentSpawnInterval && asteroids.size() < MAX_ASTEROIDS) {
        
        // Calcula quantos asteroides criar nesta leva
        int asteroidsToSpawn = std::min(
            BASE_ASTEROIDS_SPAWN + static_cast<int>(gameTime / 45), // +1 a cada 45s
            MAX_ASTEROIDS_SPAWN
        );

        // Calcula a velocidade base para esta leva
        float currentSpeed = std::min(
            BASE_ASTEROID_SPEED + (gameTime * SPEED_INCREASE_RATE),
            MAX_ASTEROID_SPEED
        );

        // Limita o spawn ao espaço disponível na tela
        size_t canSpawn = std::min(asteroidsToSpawn, MAX_ASTEROIDS - static_cast<int>(asteroids.size()));
        
        // CORREÇÃO: Usando size_t para o loop
        for (size_t i = 0; i < canSpawn; i++) {
            // Usa a variável de membro 'spawnOnLeft' para decidir o lado
            float x = spawnOnLeft 
                ? (rand() % (WIDTH / 3))                      // Lado esquerdo
                : (WIDTH * 2 / 3 + rand() % (WIDTH / 3));     // Lado direito
            
            float y = -50.0f - (i * 50.0f); // Espaçamento para não nascerem um em cima do outro
            float vx = (rand() % 100) / 100.0f - 0.5f;
            float vy = currentSpeed * (0.9f + (rand() % 20) / 100.0f); // Pequena variação de velocidade
            
            int size = (rand() % 2) + 2; // Tamanho 2 ou 3
            asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), size);
        }
        
        spawnOnLeft = !spawnOnLeft;      // Inverte o lado para a próxima leva
        asteroidSpawnClock.restart();    // Reinicia o relógio de spawn
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
        if (gameMode == GameMode::Multiplayer && player2.isAlive && player2.getBounds().intersects(asteroids[i].getBounds())) {
            player2.isAlive = false; // "Mata" o jogador 2
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

        // Balas do Jogador 2 (apenas em multiplayer)
        if (gameMode == GameMode::Multiplayer) {
            for (auto& bullet : bullets2) {
                if (bullet.isActive && bullet.getBounds().intersects(asteroids[i].getBounds())) {
                    bullet.isActive = false;
                    destroyAsteroid(i, score2); // Chama nossa função auxiliar!
                    goto next_asteroid; // Pula para o próximo asteroide
                }
            }
        }
    }
    next_asteroid:; // Rótulo para o goto
}

void GameSession::updateGameObjects(float deltaTime) {
    starfield.update(deltaTime);
    if (player1.isAlive) player1.update();
    if (player2.isAlive) player2.update();

    for (auto& bullet : bullets1) bullet.update(deltaTime);
    for (auto& bullet : bullets2) bullet.update(deltaTime);
    for (auto& asteroid : asteroids) asteroid.update(deltaTime, gameTime);
    for (auto& explosion : asteroidExplosions) explosion.timer += deltaTime;

    // Limpeza de objetos inativos (balas, explosões)
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