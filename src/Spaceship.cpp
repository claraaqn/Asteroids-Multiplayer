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
   

    // Carrega a textura apropriada para cada jogador
     if (!spaceshipSpritesheet.loadFromFile("assets/imgs/Nave_estados.png")) {
        std::cerr << "Erro ao carregar textura da nave!" << std::endl;
        exit(1);
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


// Em src/Spaceship.cpp

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

void Spaceship::update(float deltaTime) {
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
    float spriteWidth = globalBounds.width;
    float spriteHeight = globalBounds.height;
    
    const float margin = 5.0f;

    // MODIFICAÇÃO: Jogador 1 pode se mover pela tela inteira
    // Jogador 2 (se existir) fica restrito à metade direita
    if (isPlayer1) {
        // Jogador 1: sempre pode usar a tela inteira
        if (position.x < spriteWidth/2 + margin) {
            position.x = spriteWidth/2 + margin;
            velocity.x = 0;
        }
        if (position.x > WIDTH - spriteWidth/2 - margin) {
            position.x = WIDTH - spriteWidth/2 - margin;
            velocity.x = 0;
        }
    } 
    // Jogador 2 (só deve existir no multiplayer)
    else {
        if (position.x < WIDTH/2 + spriteWidth/2 + margin) {
            position.x = WIDTH/2 + spriteWidth/2 + margin;
            velocity.x = 0;
        }
        if (position.x > WIDTH - spriteWidth/2 - margin) {
            position.x = WIDTH - spriteWidth/2 - margin;
            velocity.x = 0;
        }
    }

    // Limites verticais
    if (position.y < spriteHeight/2 + margin) {
        position.y = spriteHeight/2 + margin;
        velocity.y = 0;
    }
    if (position.y > HEIGHT - spriteHeight/2 - margin) {
        position.y = HEIGHT - spriteHeight/2 - margin;
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