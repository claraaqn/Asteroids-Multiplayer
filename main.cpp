#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;
const float PI = 3.14159265f;

class Spaceship {
public:
    sf::ConvexShape shape;
    sf::Vector2f position;
    float angle;
    float speed;
    bool isAlive;
    sf::Clock fireCooldown;
    bool isPlayer1; // Para identificar qual jogador é

    Spaceship(sf::Vector2f startPos, float startAngle, bool player1) {
        position = startPos;
        angle = startAngle;
        speed = 0;
        isAlive = true;
        isPlayer1 = player1;

        shape.setPointCount(3);
        shape.setPoint(0, sf::Vector2f(20, 0));
        shape.setPoint(1, sf::Vector2f(-10, -10));
        shape.setPoint(2, sf::Vector2f(-10, 10));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(2);
        shape.setPosition(position);
        shape.setRotation(angle);
    }

    void update() {
        if (!isAlive) return;

        // Movimento apenas horizontal
        position.x += speed;

        // Limites da tela para cada jogador
        if (isPlayer1) {
            if (position.x < 0) position.x = 0;
            if (position.x > WIDTH/2) position.x = WIDTH/2;
        } else {
            if (position.x < WIDTH/2) position.x = WIDTH/2;
            if (position.x > WIDTH) position.x = WIDTH;
        }

        // Mantém a nave na parte inferior da tela
        if (position.y < HEIGHT - 50) position.y = HEIGHT - 50;
        if (position.y > HEIGHT - 30) position.y = HEIGHT - 30;

        shape.setPosition(position);
        shape.setRotation(angle);
    }

    void accelerate(float amount) {
        speed += amount;
        if (speed > 5) speed = 5;
        if (speed < -5) speed = -5;
    }

    void decelerate() {
        speed *= 0.98f;
        if (std::abs(speed) < 0.01f) speed = 0;
    }

    sf::Vector2f getFirePosition() {
        float rad = (angle - 90) * PI / 180;
        return sf::Vector2f(
            position.x + 25 * std::cos(rad),
            position.y + 25 * std::sin(rad)
        );
    }

    bool canFire() {
        return fireCooldown.getElapsedTime().asMilliseconds() > 300;
    }

    void resetFireCooldown() {
        fireCooldown.restart();
    }
};

class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool isActive;

    Bullet() {
        shape.setRadius(3);
        shape.setFillColor(sf::Color::White);
        isActive = false;
    }

    void fire(sf::Vector2f pos, float angle) {
        float rad = (angle - 90) * PI / 180;
        shape.setPosition(pos);
        velocity.x = 10 * std::cos(rad);
        velocity.y = 10 * std::sin(rad);
        isActive = true;
    }

    void update() {
        if (!isActive) return;

        shape.move(velocity);

        // Verifica se saiu da tela
        if (shape.getPosition().x < 0 || shape.getPosition().x > WIDTH ||
            shape.getPosition().y < 0 || shape.getPosition().y > HEIGHT) {
            isActive = false;
        }
    }
};

class Asteroid {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    int size; // 3 = grande, 2 = médio, 1 = pequeno

    Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz) {
        position = pos;
        velocity = vel;
        size = sz;

        shape.setRadius(size * 15);
        shape.setPointCount(size * 6 + 6);
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);
        shape.setPosition(position);
    }

    void update() {
        position += velocity;
        
        // Limites da tela
        if (position.x < -50) position.x = WIDTH + 50;
        if (position.x > WIDTH + 50) position.x = -50;
        if (position.y < -50) position.y = HEIGHT + 50;
        if (position.y > HEIGHT + 50) position.y = -50;

        shape.setPosition(position);
    }

    sf::Vector2f getPosition() const { return position; }
    float getRadius() const { return size * 15; }

private:
    sf::Vector2f position;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Asteroids Multiplayer");
    window.setFramerateLimit(60);

    // Linha divisória
    sf::RectangleShape divider(sf::Vector2f(2, HEIGHT));
    divider.setFillColor(sf::Color::White);
    divider.setPosition(WIDTH/2, 0);

    // Naves dos jogadores (agora na parte inferior)
    Spaceship player1(sf::Vector2f(WIDTH/4, HEIGHT - 40), 0, true);
    player1.shape.setOutlineColor(sf::Color::Green);

    Spaceship player2(sf::Vector2f(3*WIDTH/4, HEIGHT - 40), 0, false);
    player2.shape.setOutlineColor(sf::Color::Cyan);

    // Balas
    std::vector<Bullet> bullets1(10);
    std::vector<Bullet> bullets2(10);

    // Asteroides
    std::vector<Asteroid> asteroids;
    for (int i = 0; i < 5; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % (HEIGHT - 100); // Não spawnar muito perto das naves
        float vx = (rand() % 100) / 50.0f - 1.0f;
        float vy = (rand() % 100) / 50.0f - 1.0f;
        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), 3);
    }

    sf::Clock asteroidClock;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return EXIT_FAILURE;
    }

    sf::Text scoreText1;
    scoreText1.setFont(font);
    scoreText1.setCharacterSize(20);
    scoreText1.setFillColor(sf::Color::Green);
    scoreText1.setPosition(10, 10);

    sf::Text scoreText2;
    scoreText2.setFont(font);
    scoreText2.setCharacterSize(20);
    scoreText2.setFillColor(sf::Color::Cyan);
    scoreText2.setPosition(WIDTH - 100, 10);

    int score1 = 0;
    int score2 = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Controles do Jogador 1 (A/D para esquerda/direita, W para atirar)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            player1.accelerate(-0.2f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            player1.accelerate(0.2f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player1.canFire() && player1.isAlive) {
            for (auto& bullet : bullets1) {
                if (!bullet.isActive) {
                    bullet.fire(player1.getFirePosition(), player1.angle);
                    player1.resetFireCooldown();
                    break;
                }
            }
        }

        // Controles do Jogador 2 (setas esquerda/direita para mover, seta para cima para atirar)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player2.accelerate(-0.2f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player2.accelerate(0.2f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && player2.canFire() && player2.isAlive) {
            for (auto& bullet : bullets2) {
                if (!bullet.isActive) {
                    bullet.fire(player2.getFirePosition(), player2.angle);
                    player2.resetFireCooldown();
                    break;
                }
            }
        }

        // Desaceleração
        player1.decelerate();
        player2.decelerate();

        // Atualizações
        player1.update();
        player2.update();

        for (auto& bullet : bullets1) bullet.update();
        for (auto& bullet : bullets2) bullet.update();
        for (auto& asteroid : asteroids) asteroid.update();

        // Spawn de novos asteroides
        if (asteroidClock.getElapsedTime().asSeconds() > 5.0f && asteroids.size() < 10) {
            int side = rand() % 4;
            sf::Vector2f pos;
            if (side == 0) pos = sf::Vector2f(-50, rand() % (HEIGHT - 100));
            else if (side == 1) pos = sf::Vector2f(WIDTH + 50, rand() % (HEIGHT - 100));
            else if (side == 2) pos = sf::Vector2f(rand() % WIDTH, -50);
            else pos = sf::Vector2f(rand() % WIDTH, HEIGHT + 50);

            float vx = (rand() % 100) / 50.0f - 1.0f;
            float vy = (rand() % 100) / 50.0f - 1.0f;
            asteroids.emplace_back(pos, sf::Vector2f(vx, vy), 3);
            asteroidClock.restart();
        }

        // Colisão bala-asteroide
        for (auto& bullet : bullets1) {
            if (!bullet.isActive) continue;
            
            for (size_t i = 0; i < asteroids.size(); ++i) {
                sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                
                if (distance < asteroids[i].getRadius()) {
                    bullet.isActive = false;
                    score1 += (4 - asteroids[i].size) * 10;

                    if (asteroids[i].size > 1) {
                        for (int j = 0; j < 2; ++j) {
                            float angle = rand() % 360 * PI / 180;
                            float speed = 1 + (rand() % 100) / 50.0f;
                            sf::Vector2f vel(
                                speed * std::cos(angle),
                                speed * std::sin(angle)
                            );
                            asteroids.emplace_back(asteroids[i].getPosition(), vel, asteroids[i].size - 1);
                        }
                    }
                    
                    asteroids.erase(asteroids.begin() + i);
                    break;
                }
            }
        }

        for (auto& bullet : bullets2) {
            if (!bullet.isActive) continue;
            
            for (size_t i = 0; i < asteroids.size(); ++i) {
                sf::Vector2f diff = bullet.shape.getPosition() - asteroids[i].getPosition();
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                
                if (distance < asteroids[i].getRadius()) {
                    bullet.isActive = false;
                    score2 += (4 - asteroids[i].size) * 10;

                    if (asteroids[i].size > 1) {
                        for (int j = 0; j < 2; ++j) {
                            float angle = rand() % 360 * PI / 180;
                            float speed = 1 + (rand() % 100) / 50.0f;
                            sf::Vector2f vel(
                                speed * std::cos(angle),
                                speed * std::sin(angle)
                            );
                            asteroids.emplace_back(asteroids[i].getPosition(), vel, asteroids[i].size - 1);
                        }
                    }
                    
                    asteroids.erase(asteroids.begin() + i);
                    break;
                }
            }
        }

        // Colisão nave-asteroide
        if (player1.isAlive) {
            for (const auto& asteroid : asteroids) {
                sf::Vector2f diff = player1.position - asteroid.getPosition();
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                
                if (distance < asteroid.getRadius() + 20) {
                    player1.isAlive = false;
                    break;
                }
            }
        }

        if (player2.isAlive) {
            for (const auto& asteroid : asteroids) {
                sf::Vector2f diff = player2.position - asteroid.getPosition();
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                
                if (distance < asteroid.getRadius() + 20) {
                    player2.isAlive = false;
                    break;
                }
            }
        }

        // Atualiza placar
        scoreText1.setString("Jogador 1: " + std::to_string(score1));
        scoreText2.setString("Jogador 2: " + std::to_string(score2));

        // Renderização
        window.clear();

        // Desenha a linha divisória
        window.draw(divider);

        for (const auto& asteroid : asteroids) {
            window.draw(asteroid.shape);
        }

        if (player1.isAlive) window.draw(player1.shape);
        if (player2.isAlive) window.draw(player2.shape);

        for (const auto& bullet : bullets1) {
            if (bullet.isActive) window.draw(bullet.shape);
        }

        for (const auto& bullet : bullets2) {
            if (bullet.isActive) window.draw(bullet.shape);
        }

        window.draw(scoreText1);
        window.draw(scoreText2);

        window.display();
    }

    return 0;
}