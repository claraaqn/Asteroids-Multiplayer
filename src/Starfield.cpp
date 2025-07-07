#include "Starfield.h"
#include <cmath>

// Inicializamos os membros no construtor
Starfield::Starfield(unsigned int count, unsigned int w, unsigned int h) 
    : width(w), height(h), gen(rd()), dist(0.0f, 1.0f) {
    
    std::uniform_int_distribution<int> bright_dist(200, 255);
    std::uniform_int_distribution<int> alpha_dist(100, 255);

    stars.reserve(count);

    for (unsigned int i = 0; i < count; ++i) {
        Star star;
        
        float size = std::round(dist(gen) * 2.0f + 1.0f);
        star.shape.setSize(sf::Vector2f(size, size));
        
        star.shape.setPosition(
            dist(gen) * static_cast<float>(width),
            dist(gen) * static_cast<float>(height)
        );

        int brightness = bright_dist(gen);
        sf::Uint8 alpha = static_cast<sf::Uint8>(alpha_dist(gen));
        star.shape.setFillColor(sf::Color(brightness, brightness, brightness, alpha));
        
        star.speed = dist(gen) * 0.4f + 0.1f;
        star.scale = dist(gen);
        
        stars.push_back(star);
    }
}

void Starfield::update(float deltaTime) {
    const float pi = 3.14159265358979323846f;
    const float movementFactor = 50.0f;
    
    // Criamos distribuições locais quando necessário
    std::uniform_int_distribution<int> pos_dist(0, width);
    std::uniform_int_distribution<int> height_dist(0, 50);

    for (auto& star : stars) {
        float angle = star.scale * pi;
        float moveX = std::cos(angle) * star.speed * deltaTime * movementFactor;
        float moveY = (1.0f + std::sin(angle * 0.5f)) * star.speed * deltaTime * movementFactor;
        
        star.shape.move(moveX, moveY);
        
        auto pos = star.shape.getPosition();
        if (pos.x < -20.0f || pos.x > width + 20.0f || pos.y > height + 20.0f) {
            star.shape.setPosition(
                static_cast<float>(pos_dist(gen)),
                -static_cast<float>(height_dist(gen))
            );
        }
    }
}

void Starfield::draw(sf::RenderWindow& window) const {
    for (const auto& star : stars) {
        window.draw(star.shape);
    }
}