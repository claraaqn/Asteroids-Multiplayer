
#include "Game.h"


Game::Game() : currentState(PLAYING), player1Score(0), player2Score(0), winner(0){}

void Game::reset() {
    currentState = PLAYING;
    player1Score = 0;
    player2Score = 0;
    winner = 0;
}

bool Game::isGameOver() const {
    return currentState == GAME_OVER;
}

int Game::getWinner() const {
    return winner;
}

void Game::checkGameOver(bool player1Alive, bool player2Alive, int score1, int score2) {
    if (!player1Alive || !player2Alive) {
        currentState = GAME_OVER;  // CORREÇÃO: estava com == em vez de =
        

        // Determina o vencedor
        if (!player1Alive && !player2Alive) {
            winner = (score1 > score2) ? 1 : (score2 > score1) ? 2 : 0;
        } else {
            winner = !player1Alive ? 2 : 1;
        }
    }
}


