#include "Asteroid.h" // Inclui a declaração da classe Asteroid
#include "GameConstants.h"

using namespace GameConstants;
Asteroid::Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz) { //! isso é um construtor 

    position = sf::Vector2f(pos.x, -50); //! garante que o asteroid vem verticalmente 

    velocity = sf::Vector2f(0, vel.y); 

    size = sz; //? tamanho 

    shape.setRadius(size * 15);
    shape.setPointCount(size * 6 + 6);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);
    shape.setPosition(position);
}

void Asteroid::update() { //! atualiza a posição do asteroid a cada frame
    position += velocity;
    
    if (position.y > HEIGHT + 50) {
        position.y = -50;
        position.x = rand() % WIDTH;
    }

    shape.setPosition(position);
}

sf::Vector2f Asteroid::getPosition() const { return position; }
float Asteroid::getRadius() const { return size * 15; }
