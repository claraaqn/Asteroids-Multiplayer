#ifndef STARFIELD_H
#define STARFIELD_H

#include <SFML/Graphics.hpp>
#include <vector>

class Starfield {
private:
    struct Star {
        sf::RectangleShape shape;  
        float speed;
        float scale;
    };
    
    std::vector<Star> stars;
    int width;
    int height;
    
public:
    Starfield(int count, int w, int h);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
};

#endif // STARFIELD_H