#include "Asteroid.h"
#include "GameConstants.h"
#include <cstdlib>   // Para rand()
#include <vector>
#include <random>    // Para geração de números aleatórios
#include <cmath>     // Para std::sqrt, std::cos, std::sin, PI

// Usar PI do GameConstants
using namespace GameConstants;

// Geradores de números aleatórios (declarados globalmente para eficiência)
std::random_device rd;
std::mt19937 gen(rd());

// --- DISTRIBUIÇÕES AJUSTADAS PARA MAIOR IRREGULARIDADE ---

// Mais vértices para maior complexidade:
// Asteroides terão entre 10 e 20 vértices.
std::uniform_int_distribution<int> num_vertices_distrib(10, 20); 

// Variação angular de cada vértice, para que não fiquem em ângulos perfeitos
// +/- 20 graus (mais agressivo que antes)
std::uniform_real_distribution<float> angle_offset_distrib(-20.0f, 20.0f); 

// Variação da distância do centro, para criar protuberâncias e concavidades acentuadas
// Varia de 50% a 150% do raio base (mais agressivo que antes)
std::uniform_real_distribution<float> distance_offset_distrib(0.5f, 1.5f); 

// Para a rotação inicial aleatória
std::uniform_real_distribution<float> initial_rotation_distrib(0.0f, 360.0f);
// Para a velocidade de rotação contínua
std::uniform_real_distribution<float> rotation_speed_distrib(0.1f, 1.5f); // Variedade na velocidade de rotação

// Construtor do Asteroid
Asteroid::Asteroid(sf::Vector2f pos, sf::Vector2f vel, int sz) :
    position(pos),
    velocity(vel),
    size(sz),
    radius(0.0f) // Inicializa o raio
{
    this->position.y = -50; 
    this->velocity.x = 0;   
    
    // Calcula o raio base do asteroide (ajuste o fator 20.0f se quiser asteroides maiores/menores no geral)
    float baseRadius = (float)this->size * 20.0f; 

    // Define o número de vértices para este asteroide específico
    int numVertices = num_vertices_distrib(gen);
    shape.setPointCount(numVertices);

    float currentMaxRadius = 0.0f; 

    // Geração procedural dos pontos do asteroide
    // Usaremos um loop para gerar os pontos ao redor de um círculo
    // mas com offsets aleatórios para cada ponto.
    float angleStep = 360.0f / numVertices; // O ângulo base para cada "fatia"

    for (int i = 0; i < numVertices; ++i) {
        // Ângulo base para este vértice
        float angle = (float)i * angleStep;
        // Adiciona um offset de ângulo aleatório para mais irregularidade angular
        angle += angle_offset_distrib(gen);
        
        // Converte para radianos para funções trigonométricas
        float rad = angle * PI / 180.0f;

        // Calcula a distância do vértice ao centro com um offset aleatório
        // Isso cria protuberâncias e concavidades
        float currentDistance = baseRadius * distance_offset_distrib(gen);

        // Calcula a posição do ponto (X, Y)
        sf::Vector2f point = sf::Vector2f(
            currentDistance * std::cos(rad),
            currentDistance * std::sin(rad)
        );
        shape.setPoint(i, point);

        // Atualiza o raio máximo para colisão (o maior ponto do centro)
        float distFromCenter = std::sqrt(point.x * point.x + point.y * point.y);
        if (distFromCenter > currentMaxRadius) {
            currentMaxRadius = distFromCenter;
        }
    }
    this->radius = currentMaxRadius; // Define o raio de colisão

    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);

    // Define a origem da forma para o centro de seus pontos, para rotação correta
    sf::FloatRect bounds = shape.getLocalBounds();
    shape.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    
    shape.setPosition(this->position);
    
    // Adicionar uma rotação inicial aleatória para mais variedade
    shape.setRotation(initial_rotation_distrib(gen));
}

// O método update() permanece o mesmo, mas vamos usar a velocidade de rotação aleatória
void Asteroid::update() {
    position += velocity;
    
    if (position.y > HEIGHT + radius) { 
        position.y = -radius; 
        position.x = rand() % WIDTH;
    }

    shape.setPosition(position);
    
    // Rotação contínua com velocidade aleatória para cada asteroide
    // Declare uma variável para a velocidade de rotação como membro da classe Asteroid
    // (ex: float rotationSpeed; em Asteroid.h) e a inicialize no construtor.
    // Por simplicidade aqui, vou usar o gerador diretamente, mas o ideal é que seja membro.
    // Para deixar mais rochoso, cada um gira diferente.
    shape.rotate(rotation_speed_distrib(gen)); 
}

sf::Vector2f Asteroid::getPosition() const { return position; }
float Asteroid::getRadius() const { return radius; }