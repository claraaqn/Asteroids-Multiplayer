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
#include "Menu.h" 
#include "GameOverScreen.h"
#include "Game.h" 
#include "SuperShot.h" 
#include "NameInputScreen.h"
#include "GameMode.h"
#include "HighScoreDB.h" 

class GameSession {
public:
    // O construtor recebe o modo de jogo para saber se cria 1 ou 2 jogadores
    GameSession(sf::RenderWindow& window, sf::Font& font, GameMode mode, const sf::View& gameView, const sf::View& hudView);
    void setPlayerName(const std::string& name);
    void run();

private:
    // Métodos de lógica interna
    void handleEvents();
    void update(float deltaTime);
    void render();
    void resetGame();
    void renderGame();
    void renderHud();
    // Métodos de lógica de jogo
    void processPlayerInput(float deltaTime);
    void spawnAsteroids(float deltaTime);
    void checkCollisions();
    void updateGameObjects(float deltaTime);
    void destroyAsteroid(size_t index, int& playerScore);
    void gameOver(int finalScore);

    // Variáveis do Jogo
    sf::RenderWindow& window;
    sf::Font& font;
    GameMode gameMode;
    sf::View gameView; 
    Game gameState; 
    sf::View hudView;
    
    // variáveis da tabela
    NameInputScreen nameInputScreen;
    bool nameEntered;
    std::string currentPlayerName;
    std::string player1Name;
    std::string player2Name;
    HighScoreDB highScoreDB;

    // Entidades do Jogo
    bool spawnOnLeft;
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

    // Sistema de supertiro
    SuperShot superShot1;  // SuperShot do jogador 1
    SuperShot superShot2;  // SuperShot do jogador 2
    sf::Text superShotText1;
    sf::Text superShotText2;
};