#pragma once
#include <SFML/Graphics.hpp>

class Asteroid {
private:
    // Declare na mesma ordem que serão inicializados no construtor
    sf::Vector2f position;
    sf::Vector2f velocity;
    int size;
    float radius;
    float spawnTime;
    float growthTime;
    float scale;
    float targetScale;
    float rotationSpeed;
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded = false;
    
public:
    Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz);
    void update(float deltaTime, float currentTime);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2f getPosition() const;
    float getRadius() const;
    int getSize() const ;

    
};