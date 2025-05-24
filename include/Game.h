#pragma once
#include <SFML/Graphics.hpp>

class Game {
public:
    enum State { PLAYING, GAME_OVER };
    
    State currentState;
    int player1Score;
    int player2Score;

    Game();
    void reset();
    bool isGameOver() const;
    void checkGameOver(bool player1Alive, bool player2Alive);
};