#pragma once

class Game {
public:
    enum GameState { PLAYING, GAME_OVER };
    
    Game();
    void reset();
    bool isGameOver() const;
    void checkGameOver(bool player1Alive, bool player2Alive, int score1, int score2);
    int getWinner() const ;
    
private:
    GameState currentState;
    int player1Score;
    int player2Score;
    int winner; 
};