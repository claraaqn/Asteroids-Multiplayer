// GameSession.h
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Spaceship.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "Starfield.h"
#include "AsteroidExplosion.h"
#include "Menu.h" // Para o GameMode
#include "GameOverScreen.h"
#include "Game.h" // Para o estado de Game Over

class GameSession {
public:
    // O construtor recebe o modo de jogo para saber se cria 1 ou 2 jogadores
    GameSession(sf::RenderWindow& window, sf::Font& font, GameMode mode);
    void run(); 

private:
    // Métodos de lógica interna
    void handleEvents();
    void update(float deltaTime);
    void render();
    void resetGame();

    // Métodos de lógica de jogo
    void processPlayerInput(float deltaTime);
    void spawnAsteroids(float deltaTime);
    void checkCollisions();
    void updateGameObjects(float deltaTime);
    void destroyAsteroid(size_t index, int& playerScore);

    // Variáveis do Jogo
    sf::RenderWindow& window;
    sf::Font& font;
    GameMode gameMode;
    Game gameState; // Gerencia o estado de game over
     // Relógio para controlar o tempo entre spawns
    bool spawnOnLeft;

    // Entidades do Jogo
    Spaceship player1;
    Spaceship player2;
    std::vector<Bullet> bullets1;
    std::vector<Bullet> bullets2;
    std::vector<Asteroid> asteroids;
    std::vector<AsteroidExplosion> asteroidExplosions;
    Starfield starfield;

    // UI & Efeitos
    sf::RectangleShape divider;
    sf::Text scoreText1;
    sf::Text scoreText2;
    int score1, score2;

    // Sons
    sf::SoundBuffer shootBuffer;
    sf::SoundBuffer explosionBuffer;
    std::vector<sf::Sound> activeSounds;

    // Controles de Dificuldade e Tempo
    sf::Clock gameClock;
    sf::Clock asteroidSpawnClock;
    float gameTime;

    // Tela de Game Over
    GameOverScreen gameOverScreen;
};