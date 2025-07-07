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
std::uniform_int_distribution<int> num_vertices_distrib(10, 20);
std::uniform_real_distribution<float> angle_offset_distrib(-20.0f, 20.0f);
std::uniform_real_distribution<float> distance_offset_distrib(0.5f, 1.5f);
std::uniform_real_distribution<float> initial_rotation_distrib(0.0f, 360.0f);
std::uniform_real_distribution<float> rotation_speed_distrib(0.1f, 1.5f);


Asteroid::Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz) :
    position(pos),
    velocity(vel),
    size(sz),
    radius(0.0f),
    spawnTime(0.0f),
    growthTime(1.0f),
    scale(0.1f),
    targetScale(1.0f),
    rotationSpeed(rotation_speed_distrib(gen))
{
    if (!texture.loadFromFile("assets/imgs/Asteroid.png")) {
        std::cerr << "Erro ao carregar textura do asteroide!" << std::endl;
        // Fallback: cria um círculo vermelho para debug
        sprite.setColor(sf::Color::Red);
    }
    texture.setSmooth(false); 

    // Configura a sprite
    sprite.setTexture(texture);
    sprite.setColor(sf::Color::White);
    sf::FloatRect bounds = sprite.getLocalBounds();
    radius = (bounds.width + bounds.height) / 4.0f * size;

    
    sprite.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    sprite.setPosition(this->position);
    sprite.setRotation(initial_rotation_distrib(gen)); 
    sprite.setScale(size, size);
}

void Asteroid::update(float deltaTime, float currentTime) {
    position += velocity * deltaTime;
    sprite.rotate(rotationSpeed * deltaTime);
    
    if (spawnTime > 0 && scale < targetScale) {
        scale = std::min(targetScale, scale + deltaTime/growthTime);
        sprite.setScale(scale * size, scale * size);
    }
    
    sprite.setPosition(position);
    
}

sf::Vector2f Asteroid::getPosition() const {
    return position;
}

float Asteroid::getRadius() const {
    return radius * scale;
}

void Asteroid::draw(sf::RenderWindow& window) const {
    window.draw(sprite); // Desenha o shape do asteroide na janela
}

int Asteroid::getSize() const {
    return size;
}