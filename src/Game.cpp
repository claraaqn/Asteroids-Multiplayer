
#include "Game.h"


Game::Game() : currentState(PLAYING), player1Score(0), player2Score(0){}

void Game::reset() {
    currentState = PLAYING;
    player1Score = 0;
    player2Score = 0;
}

bool Game::isGameOver() const {
    return currentState == GAME_OVER;
}

void Game::checkGameOver(bool player1Alive, bool player2Alive) {
    if ((!player1Alive || !player2Alive) && currentState == PLAYING) {
        currentState = GAME_OVER;
    }
}

