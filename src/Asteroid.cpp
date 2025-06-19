#include "Asteroid.h"
#include "GameConstants.h"
#include <cstdlib>
#include <vector>
#include <random>
#include <cmath>

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
    scale(1.0f),
    targetScale(1.0f),
    rotationSpeed(rotation_speed_distrib(gen))
{
    float baseRadius = (float)this->size * 20.0f;
    int numVertices = num_vertices_distrib(gen);
    shape.setPointCount(numVertices);

    float currentMaxRadius = 0.0f;
    float angleStep = 360.0f / numVertices;

    for (int i = 0; i < numVertices; ++i) {
        float angle = (float)i * angleStep;
        angle += angle_offset_distrib(gen);
        float rad = angle * PI / 180.0f;
        float currentDistance = baseRadius * distance_offset_distrib(gen);

        sf::Vector2f point = sf::Vector2f(
            currentDistance * std::cos(rad),
            currentDistance * std::sin(rad)
        );
        shape.setPoint(i, point);

        float distFromCenter = std::sqrt(point.x * point.x + point.y * point.y);
        if (distFromCenter > currentMaxRadius) {
            currentMaxRadius = distFromCenter;
        }
    }
    this->radius = currentMaxRadius;

    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);

    sf::FloatRect bounds = shape.getLocalBounds();
    shape.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    shape.setPosition(this->position);
    shape.setRotation(initial_rotation_distrib(gen));
}

void Asteroid::update(float deltaTime, float currentTime) {
    position += velocity * deltaTime;
    shape.rotate(rotationSpeed * deltaTime);
    
    if (spawnTime > 0) {
        float timeSinceSpawn = currentTime - spawnTime;
        if (timeSinceSpawn < growthTime) {
            scale = 0.1f + (targetScale - 0.1f) * (timeSinceSpawn / growthTime);
        } else {
            scale = targetScale;
            spawnTime = -1.0f;
        }
    }
    
    shape.setPosition(position);
    shape.setScale(scale, scale);
}

sf::Vector2f Asteroid::getPosition() const {
    return position;
}

float Asteroid::getRadius() const {
    return radius * scale;
}

void Asteroid::draw(sf::RenderWindow& window) const {
    window.draw(shape); // Desenha o shape do asteroide na janela
}