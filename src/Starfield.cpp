#include "Starfield.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

Starfield::Starfield(int count, int w, int h) : width(w), height(h) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    for (int i = 0; i < count; ++i) {
        Star star;
        // Cria um quadrado pequeno (tamanho entre 1x1 e 3x3 pixels)
        float size = static_cast<float>(rand() % 3 + 1);
        star.shape.setSize(sf::Vector2f(size, size));
        
        // Posição aleatória
        star.shape.setPosition(
            static_cast<float>(rand() % width),
            static_cast<float>(rand() % height)
        );

        
        int brightness = rand() % 55 + 200; // Estrelas mais brilhantes (200-255)
       
        
        // Cor com transparência aleatória
        star.shape.setFillColor(sf::Color(
            255, 255, 255, 
            static_cast<sf::Uint8>(rand() % 155 + 100)  // Alpha entre 100-255
        ));
        
        // Velocidade aleatória
        star.speed = static_cast<float>(rand() % 5 + 1) / 10.0f; // 0.1-0.5
        star.scale = static_cast<float>(rand() % 100) / 100.0f;
        stars.push_back(star);
    }
}

void Starfield::update(float deltaTime) {
    for (auto& star : stars) {
        // Movimento em direção aleatória (com tendência para baixo)
        float angle = (star.scale * 0.5f) * 3.14159f; // Variação de ângulo
        float moveX = std::cos(angle) * star.speed * deltaTime;
        float moveY = (1.0f + std::sin(angle)) * star.speed * deltaTime;
        
        star.shape.move(moveX, moveY);
        
        // Se sair da tela, reposiciona
        sf::Vector2f pos = star.shape.getPosition();
        if (pos.x < -10 || pos.x > width + 10 || pos.y > height + 10) {
            star.shape.setPosition(
                static_cast<float>(rand() % width),
                static_cast<float>(-rand() % 50) // Aparece acima da tela
            );
        }
    }
}

void Starfield::draw(sf::RenderWindow& window) {
    for (auto& star : stars) {
        window.draw(star.shape);
    }
}