// Asteroid.h
#pragma once
#include <SFML/Graphics.hpp>

class Asteroid {
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    int size;
    float baseRadius;  // Raio base sem escala
    float currentRadius; // Raio atual considerando escala
    float spawnTime;
    float growthTime;
    float scale;
    float targetScale;
    float rotationSpeed;
    static sf::Texture texture;
    static bool textureLoaded;
    sf::Sprite sprite;

    sf::CircleShape collisionRadiusVisual; // Novo: visualização do raio
    bool showRadius;
    
public:
    Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz);
    void update(float deltaTime, float currentTime);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2f getPosition() const;
    float getRadius() const { return currentRadius; }
    int getSize() const { return size; }
    const sf::Sprite& getSprite() const { return sprite; } 
     void toggleRadiusVisual(bool show);
};