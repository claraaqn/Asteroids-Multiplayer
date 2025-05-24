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
    sf::Vector2f velocity;
    bool isAlive;
    sf::Clock fireCooldown;
    bool isPlayer1; // Para identificar qual jogador é

    Spaceship(sf::Vector2f startPos, float startAngle, bool player1) {
        position = startPos;
        angle = startAngle;
        velocity = sf::Vector2f(0, 0);
        isAlive = true;
        isPlayer1 = player1;

        shape.setPointCount(7);

        shape.setPoint(0, sf::Vector2f(15, 0));    // Ponta da nave
        shape.setPoint(1, sf::Vector2f(-10, -10)); // Asa superior traseira
        shape.setPoint(2, sf::Vector2f(-5, -5));   // Curva superior
        shape.setPoint(3, sf::Vector2f(-10, 0));   // Centro traseiro
        shape.setPoint(4, sf::Vector2f(-5, 5));    // Curva inferior
        shape.setPoint(5, sf::Vector2f(-10, 10));  // Asa inferior traseira
        shape.setPoint(6, sf::Vector2f(15, 0));    // Fecha o polígono

        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(2);
        shape.setPosition(position);
        shape.setRotation(angle);
        float sumX = 0;
        float sumY = 0;
        int count = shape.getPointCount();

        for (int i = 0; i < count; i++) {
            sf::Vector2f p = shape.getPoint(i);
            sumX += p.x;
            sumY += p.y;
        }

        float centerX = sumX / count;
        float centerY = sumY / count;

        shape.setOrigin(centerX, centerY);

    }

    void update() {
        if (!isAlive) return;

        // Movimento apenas horizontal
        position.x += velocity.x;
        position.y += velocity.y;

        // Limites da tela para cada jogador
        if (isPlayer1) {
            if (position.x < 0) position.x = 0;
            if (position.x > WIDTH/2) position.x = WIDTH/2;
        } else {
            if (position.x < WIDTH/2) position.x = WIDTH/2;
            if (position.x > WIDTH) position.x = WIDTH;
        }

        // Mantém a nave na parte inferior da tela
        if (position.y < 0) position.y = 0;
        if (position.y > HEIGHT) position.y = HEIGHT;

        shape.setPosition(position);
        shape.setRotation(angle);
    }

    void accelerate(float amount) {
        // Conversão de ângulo para vetor de aceleração
        float rad = (angle - 90) * PI / 180;
        sf::Vector2f acceleration(
            amount * std::cos(rad),
            amount * std::sin(rad)
        );
        
        velocity += acceleration;
        
        // Limite de velocidade
        float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (speed > 5.0f) {
            velocity = (velocity / speed) * 5.0f;
        }
    }

     void decelerate() {
        velocity *= 0.98f;  // Desaceleração suave
        
        // Parada completa quando muito lento
        if (std::abs(velocity.x) < 0.01f && std::abs(velocity.y) < 0.01f) {
            velocity = sf::Vector2f(0, 0);
        }
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
        shape.setOrigin(0, 0); // Substituir por uma origem melhor

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

        sf::Vector2f previousPosition = shape.getPosition();
        shape.move(velocity);
        sf::Vector2f currentPosition = shape.getPosition();

        // Verifica se saiu da tela
        if (currentPosition.x < 0 || currentPosition.x > WIDTH ||
            currentPosition.y < 0 || currentPosition.y > HEIGHT) {
            isActive = false;
            return;
        }

        // Verifica se cruzou a linha divisória (só verifica se houve mudança de lado)
        if ((previousPosition.x < WIDTH/2 && currentPosition.x >= WIDTH/2) || 
            (previousPosition.x >= WIDTH/2 && currentPosition.x < WIDTH/2)) {
            isActive = false;
        }
    }
};

class Asteroid {
public:
    sf::CircleShape shape; //? aparência visual do asteroide
    sf::Vector2f velocity; //? velocidade do asteroide (direção e magnitude)
    int size; //? 3 = grande, 2 = médio, 1 = pequeno

    Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz) { //! isso é um construtor 

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

    void update() { //! atualiza a posição do asteroid a cada frame
        position += velocity;
        
        if (position.y > HEIGHT + 50) {
            position.y = -50;
            position.x = rand() % WIDTH;
        }

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

    for (auto& bullet : bullets1) {
        bullet.shape.setFillColor(sf::Color::Green);
    }

    // Balas do jogador 2 (ciano)
    for (auto& bullet : bullets2) {
        bullet.shape.setFillColor(sf::Color::Cyan);
    }

    //! são os asteroids iniciais - assim que o jogo começa
    std::vector<Asteroid> asteroids;
    for (int i = 0; i < 5; ++i) {
        float x = rand() % WIDTH; // Posição X aleatória na largura da tela
        float y = -50.0f; // Spawna acima da tela (fora da visão)

        // Tamanho aleatório (3-grande, 2-médio, 1-pequeno)
        int size = 1 + (rand() % 3); // 1, 2 ou 3
        
        // Velocidade controlada (só para baixo, com variação)
        float vx = 0.0f; // Sem movimento horizontal
        float vy = ((rand() % 100) + (size * 10.0f)) / (40.0f); //! quanto maior + rápido
        
        asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(vx, vy), size);
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

        // Sistema de tiro por evento (melhor para tiros únicos)
        if (event.type == sf::Event::KeyPressed) {
            // Jogador 1 - ESPAÇO
            
            if (event.key.code == sf::Keyboard::Space && player1.canFire() && player1.isAlive) {
                for (auto& bullet : bullets1) {
                    if (!bullet.isActive) {
                        bullet.fire(player1.getFirePosition(), player1.angle);
                        player1.resetFireCooldown();
                        break;
                    }

                }
            }
            // Jogador 2 - ENTER (ou use sf::Keyboard::Return)
        if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) && 
            player2.canFire() && player2.isAlive){                
                for (auto& bullet : bullets2) {
                    if (!bullet.isActive) {
                        bullet.fire(player2.getFirePosition(), player2.angle);
                        player2.resetFireCooldown();
                        break;
                    }
                }
            }
        }
    }
    for (const auto& bullet : bullets1) {
    if (bullet.isActive)
        window.draw(bullet.shape);
    }
    for (const auto& bullet : bullets2) {
        if (bullet.isActive)
            window.draw(bullet.shape);
    }



        // Controles do Jogador 1 (A/D para esquerda/direita, W para atirar)
        // Controles do Jogador 1 (WASD)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        player1.angle -= 3.0f;  // Gira para esquerda
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        player1.angle += 3.0f;  // Gira para direita
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        player1.accelerate(0.1f);  // Acelera na direção atual
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        player1.accelerate(-0.05f);  // Freio/reverso
    }

    // Controles do Jogador 2 (Setas)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player2.angle -= 3.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player2.angle += 3.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        player2.accelerate(0.1f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        player2.accelerate(-0.05f);
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

        //! Spawn de novos asteroides - novos asteroids 
        if (asteroidClock.getElapsedTime().asSeconds() > 2.0f) {  // Ajuste o tempo para mais/ menos asteroides
            float x = rand() % WIDTH;      // Posição X aleatória
            float y = -50.0f;              // Sempre spawna de cima

            // Tamanho aleatório (1-pequeno, 2-médio, 3-grande)
            int size = 1 + (rand() % 3);  

            // Velocidade: Quanto maior o asteroide, mais rápido
            float vy = 1.0f + (size * 0.5f) + (rand() % 20) / 20.0f;  

            asteroids.emplace_back(sf::Vector2f(x, y), sf::Vector2f(0.0f, vy), size);
            asteroidClock.restart();  // Reinicia o timer
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

        scoreText1.setString("P1: " + std::to_string(score1));
        scoreText2.setString("P2: " + std::to_string(score2));


        window.draw(scoreText1);
        window.draw(scoreText2);

        window.display();
    }

    return 0;
}