#pragma once
#include <SFML/Graphics.hpp>

class Asteroid {
public:
    sf::ConvexShape shape;
    sf::Vector2f velocity;
    int size;

    Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz);
    void update();
    sf::Vector2f getPosition() const;
    float getRadius() const;

private:
    sf::Vector2f position;
    float radius;
};