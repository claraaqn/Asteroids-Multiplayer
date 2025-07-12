// Asteroid.cpp
#include "Asteroid.h"
#include "GameConstants.h"
#include <cstdlib>
#include <vector>
#include <random>
#include <cmath>
#include <iostream>

using namespace GameConstants;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> initial_rotation_distrib(0.0f, 360.0f);
std::uniform_real_distribution<float> rotation_speed_distrib(0.1f, 1.5f);
sf::Texture Asteroid::texture;
bool Asteroid::textureLoaded = false;

Asteroid::Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz) :
    position(pos),
    velocity(vel),
    size(sz),
    spawnTime(0.0f),
    growthTime(1.0f),
    scale(0.1f),
    targetScale(1.0f),
    rotationSpeed(rotation_speed_distrib(gen))
{
    if (!textureLoaded) {
        // Modifique o caminho para tentar alternativas
        if (!texture.loadFromFile("assets/imgs/Asteroid.png") && 
            !texture.loadFromFile("assets/imgs/asteroid.png")) {
            std::cerr << "ERRO: Não foi possível carregar a textura do asteroide!" << std::endl;
            // Criar um fallback visual
            texture.create(64, 64);
            sf::Uint8* pixels = new sf::Uint8[64 * 64 * 4];
            for (int i = 0; i < 64 * 64 * 4; i += 4) {
                pixels[i] = 100;   // R
                pixels[i+1] = 100; // G
                pixels[i+2] = 100; // B
                pixels[i+3] = 255; // A
            }
            texture.update(pixels);
            delete[] pixels;
        }
        texture.setSmooth(false);
        textureLoaded = true;
    }

    sprite.setTexture(texture, true); // O segundo parâmetro 'true' reseta o retângulo de textura
    sf::FloatRect bounds = sprite.getLocalBounds();
    
    // Ajuste no cálculo do raio
    baseRadius = std::min(bounds.width, bounds.height) / 2.0f;
    currentRadius = baseRadius * size;
    
    // Centralização precisa
    sprite.setOrigin(bounds.width/2.0f, bounds.height/2.0f);
    sprite.setPosition(position);
    sprite.setRotation(initial_rotation_distrib(gen));
    
    // Ajuste na escala inicial
    sprite.setScale(size * scale, size * scale);

}

void Asteroid::update(float deltaTime, float currentTime) {
    position += velocity * deltaTime;
    sprite.rotate(rotationSpeed * deltaTime);
    
    // Animação de crescimento
    if (scale < targetScale) {
        scale = std::min(targetScale, scale + deltaTime/growthTime);
        sprite.setScale(size * scale, size * scale);
        currentRadius = baseRadius * size * scale;
    }
    
    sprite.setPosition(position);
    
    // Debug: verifique se está na tela
    if (position.y > HEIGHT + 100 || position.y < -100 || 
        position.x > WIDTH + 100 || position.x < -100) {
    }
}

void Asteroid::draw(sf::RenderWindow& window) const {
    // Debug: verifique antes de desenhar
    sf::FloatRect bounds = sprite.getGlobalBounds();
    if (bounds.width <= 0 || bounds.height <= 0) {
        std::cerr << "ERRO: Tentando desenhar asteroide com tamanho inválido!" << std::endl;
        return;
    }
    window.draw(sprite);
}

sf::Vector2f Asteroid::getPosition() const {
    return position;
}
