#pragma once
#include <SFML/Graphics.hpp>

struct AsteroidExplosion {
    sf::Vector2f position;
    float timer;
    int originalSize;
};