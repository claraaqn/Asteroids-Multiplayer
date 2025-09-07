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
    sf::FloatRect bounds = sprite.getLocalBounds();

    fireCooldown.restart();
   

    // Carrega a textura apropriada para cada jogador
    if (isPlayer1) {
        if (!texture.loadFromFile("assets/imgs/Nave.png")) {
            std::cerr << "Erro ao carregar textura da nave do jogador 1!" << std::endl;
        }
    } else {
        if (!texture.loadFromFile("assets/imgs/Nave2.png")) {
            std::cerr << "Erro ao carregar textura da nave do jogador 2!" << std::endl;
        }
    }

    sprite.setTexture(texture);
    sprite.setScale(SCALE_FACTOR *1.6f, SCALE_FACTOR*1.6f);
    // Configura a sprite
    sprite.setOrigin(bounds.width/2 +14, bounds.height/2);
    sprite.setPosition(position);
    sprite.setRotation(angle);
    

}

void Spaceship::update() {
    if (!isAlive) return;
    
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