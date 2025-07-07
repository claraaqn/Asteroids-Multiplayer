#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>  // Adicionado aqui

class Starfield {
public:
    Starfield(unsigned int count, unsigned int width, unsigned int height);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;

private:
    struct Star {
        sf::RectangleShape shape;
        float speed;
        float scale;
    };

    std::vector<Star> stars;
    unsigned int width;
    unsigned int height;
    
    // Adicionamos os geradores de números aleatórios como membros
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist;
};