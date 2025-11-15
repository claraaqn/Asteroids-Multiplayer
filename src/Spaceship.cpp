#include "Spaceship.h" // Inclui a DECLARAÇÃO da classe Spaceship
#include <cmath> // Para std::cos, std::sin
#include <iostream>
using namespace GameConstants;

Spaceship::Spaceship(sf::Vector2f startPos, float startAngle, bool player1) {
    position = startPos;
    angle = startAngle;
    velocity = sf::Vector2f(0, 0);
    isAlive = true;
    isPlayer1 = player1;
    isAccelerating = false;
    fireCooldown.restart();
    gameMode = GameMode::SinglePlayer;
   

    // Carrega a textura apropriada para cada jogador
     if (isPlayer1) {
        // Player 1 - nave original
        if (!spaceshipSpritesheet.loadFromFile("assets/imgs/Nave_estados.png")) {
            std::cerr << "Erro ao carregar textura da nave do player 1!" << std::endl;
            exit(1);
        }
    } else {
        // Player 2 - nova sprite
        if (!spaceshipSpritesheet.loadFromFile("assets/imgs/Nave2_estados.png")) {
            std::cerr << "Erro ao carregar textura da nave do player 2!" << std::endl;
            // Fallback para a textura do player 1 se a do player 2 não existir
            if (!spaceshipSpritesheet.loadFromFile("assets/imgs/Nave_estados.png")) {
                exit(1);
            }
        }
    }

    // 3. Define as dimensões dos frames e a lógica da spritesheet
    // Baseado na sua imagem:
    frameWidth = 96; // Largura de cada "célula" na spritesheet
    frameHeight = 96; // Altura de cada "célula" na spritesheet
    totalFramesNormal = 1; // A nave normal tem 1 frame
    totalFramesAccelerating = 3; // A nave acelerando tem 3 frames de fogo (excluindo a normal)

    currentRowNormal = 0; // A nave normal está na primeira linha (índice 0)
    currentRowAccelerating = 0;

    sprite.setTexture(spaceshipSpritesheet);
    sprite.setTextureRect(sf::IntRect(0, currentRowNormal * frameHeight, frameWidth, frameHeight));

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width/2.f, bounds.height/2.f);
    sprite.setScale(SCALE_FACTOR *1.6f, SCALE_FACTOR*1.6f);

    sprite.setPosition(position);
    sprite.setRotation(angle);

    // 5. Inicializa variáveis da ANIMAÇÃO
    animationTimer = 0.0f;
    animationSpeed = 0.1f;  // Troca de frame a cada 0.1 segundos
    currentFrame = 0;

}

void Spaceship::setAccelerating(bool accelerating) {
    // Se o estado não mudou, não faz nada. Isso é crucial!
    if (isAccelerating == accelerating) {
        return;
    }

    isAccelerating = accelerating;
    animationTimer = 0.0f; // Reinicia o timer para a nova animação começar do zero
    currentFrame = 0;      // Reinicia a contagem de frames
     if (isAccelerating) {
        // Quando começa a acelerar, define para o PRIMEIRO frame de fogo.
        sprite.setTextureRect(sf::IntRect(frameWidth, currentRowAccelerating * frameHeight, frameWidth, frameHeight));
    } else {
        // Quando para de acelerar, define para o frame normal.
        sprite.setTextureRect(sf::IntRect(0, currentRowNormal * frameHeight, frameWidth, frameHeight));
    }
}

void Spaceship::draw(sf::RenderWindow& window) {
    // Primeiro, desenha a própria nave
    window.draw(sprite);

    // Se a nave estiver acelerando, desenha o fogo

}

void Spaceship::update(float deltaTime,  bool isSingleplayer) {
    if (!isAlive) return;

    // --- LÓGICA DE ANIMAÇÃO CENTRALIZADA ---
    animationTimer += deltaTime;

    if (isAccelerating) {
        // Se está acelerando, anima os frames do propulsor
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f; // Reseta o timer

            // Incrementa currentFrame e o limita ao número total de frames de aceleração (3 frames: 0, 1, 2)
            currentFrame = (currentFrame + 1) % totalFramesAccelerating; // Isso fará 0 -> 1 -> 2 -> 0 -> ...

            // Com base no currentFrame (0, 1 ou 2), escolhe o retângulo correto na spritesheet 2x2
            int frameX = 0;
            int frameY = 0;

            // Ajuste o switch para corresponder aos frames de fogo da sua imagem:
            // Frame 0 da ANIMAÇÃO (primeiro de fogo) -> Canto Superior Direito
            // Frame 1 da ANIMAÇÃO (segundo de fogo)  -> Canto Inferior Esquerdo
            // Frame 2 da ANIMAÇÃO (terceiro de fogo) -> Canto Inferior Direito
            switch (currentFrame) {
                case 0: // Primeiro frame de fogo (canto superior direito)
                    frameX = frameWidth;
                    frameY = 0;
                    break;
                case 1: // Segundo frame de fogo (canto inferior esquerdo)
                    frameX = 0;
                    frameY = frameHeight;
                    break;
                case 2: // Terceiro frame de fogo (canto inferior direito)
                    frameX = frameWidth;
                    frameY = frameHeight;
                    break;
            }

            sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
        }
    } else {
        // Se NÃO está acelerando, garante que a nave mostre o frame normal (canto superior esquerdo)
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    }

    // Atualiza posição
    position += velocity;
    
    // Obtém as dimensões reais da sprite
    sf::FloatRect globalBounds = sprite.getGlobalBounds();
    float halfSpriteWidth = (globalBounds.width / 2.0f) * 0.6f;
    float halfSpriteHeight = (globalBounds.height / 2.0f) * 0.2f;
    
    const float margin = 2.0f;

    if (isPlayer1) {
        // Jogador 1
        if (isSingleplayer) {
            // Singleplayer - tela inteira
            if (position.x < halfSpriteWidth + margin) {
                position.x = halfSpriteWidth + margin;
                velocity.x = 0;
            }
            if (position.x > WIDTH - halfSpriteWidth - margin) {
                position.x = WIDTH - halfSpriteWidth - margin;
                velocity.x = 0;
            }
        } else {
            // Multiplayer - metade ESQUERDA (de 0 até WIDTH/2)
            if (position.x < halfSpriteWidth + margin) {
                position.x = halfSpriteWidth + margin;
                velocity.x = 0;
            }
            if (position.x > (WIDTH/2) - halfSpriteWidth - margin) {
                position.x = (WIDTH/2) - halfSpriteWidth - margin;
                velocity.x = 0;
            }
        }
    } 
    // Jogador 2 (só existe no multiplayer)
    else {
        // Multiplayer - metade DIREITA (de WIDTH/2 até WIDTH)
        if (position.x < (WIDTH/2) + halfSpriteWidth + margin) {
            position.x = (WIDTH/2) + halfSpriteWidth + margin;
            velocity.x = 0;
        }
        if (position.x > WIDTH - halfSpriteWidth - margin) {
            position.x = WIDTH - halfSpriteWidth - margin;
            velocity.x = 0;
        }
    }

    // Limites verticais
    if (position.y < halfSpriteHeight + margin) {
        position.y = halfSpriteHeight + margin;
        velocity.y = 0;
    }
    if (position.y > HEIGHT - halfSpriteHeight - margin) {
        position.y = HEIGHT - halfSpriteHeight - margin;
        velocity.y = 0;
    }

    sprite.setPosition(position);
    sprite.setRotation(angle);
}

void Spaceship::accelerate(float amount) {
    // Conversão de ângulo para vetor de aceleração
    float rad = (sprite.getRotation() - 90.0f) * PI / 180.0f;
        sf::Vector2f acceleration(
            amount * 0.5f * std::cos(rad),
            amount * 0.5f * std::sin(rad)
        );
    
    velocity += acceleration;
    
    // Limite de velocidade
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > 5.0f) {
        velocity = (velocity / speed) * 5.0f;
    }
}

void Spaceship::decelerate() {
    velocity *= 0.98f;  // Desaceleração suave

    // Parada completa quando muito lento
    if (std::abs(velocity.x) < 0.01f && std::abs(velocity.y) < 0.01f) {
        velocity = sf::Vector2f(0, 0);
    }
}

sf::Vector2f Spaceship::getFirePosition() const {
float rad = (sprite.getRotation() - 90.0f) * PI / 180.0f;

    return sf::Vector2f(
        (position.x + 25 * std::cos(rad)),
        (position.y + 25 * std::sin(rad))
    );

}

bool Spaceship::canFire() const {
    return fireCooldown.getElapsedTime().asMilliseconds() > FIRE_COOLDOWN;
}

void Spaceship::resetFireCooldown()  {
    fireCooldown.restart();
}

void Spaceship::reset(sf::Vector2f newPosition, float newAngle, bool player) {
    position = newPosition;
    angle = newAngle;
    velocity = sf::Vector2f(0, 0);
    isAlive = true;
    isPlayer1 = player;
    
    // Reset da sprite
    sprite.setPosition(position);
    sprite.setRotation(angle);
    
    
    fireCooldown.restart();
}

sf::FloatRect Spaceship::getBounds() const {
    return sprite.getGlobalBounds();
}

float Spaceship::getCollisionRadius() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return std::min(bounds.width, bounds.height) * 0.15f; 
}