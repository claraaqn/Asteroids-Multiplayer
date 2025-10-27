#pragma once
#include <SFML/Graphics.hpp>
#include "GameConstants.h"

class Spaceship {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f position;
    float angle;
    sf::Vector2f velocity;
    bool isAlive;
    bool isPlayer1;
    float fireCooldownMax;
    sf::FloatRect getBounds() const;

    Spaceship(sf::Vector2f startPos, float startAngle, bool player1);
    void update(float deltaTime);
    void accelerate(float amount);
    void setAccelerating(bool accelerating);
    void draw(sf::RenderWindow& window);
    void decelerate();
    sf::Vector2f getFirePosition() const;
    bool canFire() const;
    void resetFireCooldown();
    void reset(sf::Vector2f newPosition, float newAngle, bool player);
    float getCollisionRadius() const;

private:
    static constexpr float FIRE_COOLDOWN = 300.0f;
    sf::Clock fireCooldown;
    sf::Texture spaceshipSpritesheet;
    bool isAccelerating;
    // --- VARIÁVEIS PARA ANIMAÇÃO ---
    float animationTimer;
    float animationSpeed;    // Tempo para cada frame (e.g., 0.1s)
    int currentFrame;
    int frameWidth;          // Largura de um único frame na spritesheet
    int frameHeight;         // Altura de um único frame
    int totalFramesNormal;   // Número de frames para a nave normal
    int totalFramesAccelerating; // Número de frames para a nave acelerando
    int currentRowNormal;    // Linha onde estão os frames normais (e.g., 0)
    int currentRowAccelerating; // Linha onde estão os frames acelerando (e.g., 1)


};