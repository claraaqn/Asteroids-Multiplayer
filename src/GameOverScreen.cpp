#include "GameOverScreen.h"

GameOverScreen::GameOverScreen(const sf::Font& font) {
    // Configuração dos textos
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER!");
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(sf::Color::White);
    
    winnerText.setFont(font);
    winnerText.setCharacterSize(30);
    
    restartText.setFont(font);
    restartText.setString("PRESS 'R' TO RESTART");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);

    quitText.setFont(font);
    quitText.setString("PRESS 'Q' TO QUIT");
    quitText.setCharacterSize(30);
    quitText.setFillColor(sf::Color::White);
    
    
    // Centraliza os textos
    centerText(gameOverText);
    centerText(winnerText);
    centerText(restartText);
    centerText(quitText);

    // Posicionamento relativo
    gameOverText.setPosition(0, -120);
    winnerText.setPosition(0, 0);
    restartText.setPosition(0, 100);
    quitText.setPosition(0, 160); 
}

void GameOverScreen::update(int winner, int score1, int score2) {
    if (winner == 0) {
        winnerText.setString("EMPATE!");
        winnerText.setFillColor(sf::Color::Yellow);
    } 
    else if (winner == 1) {
        winnerText.setString("JOGADOR 1 GANHOU!\nPontos: " + std::to_string(score1));
        winnerText.setFillColor(sf::Color::Green);
    } 
    else {
        winnerText.setString("JOGADOR 2 GANHOU!\nPontos: " + std::to_string(score2));
        winnerText.setFillColor(sf::Color::Cyan);
    }
    
    centerText(winnerText);
}

void GameOverScreen::draw(sf::RenderWindow& window) {
    window.draw(gameOverText);
    window.draw(winnerText);
    window.draw(restartText);
    window.draw(quitText);
}

void GameOverScreen::setPosition(float x, float y) {
    gameOverText.setPosition(x, y + gameOverText.getPosition().y);
    winnerText.setPosition(x, y + winnerText.getPosition().y);
    restartText.setPosition(x, y + restartText.getPosition().y);
    quitText.setPosition(x, y + quitText.getPosition().y);
}

void GameOverScreen::centerText(sf::Text& text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width/2, bounds.height/2);
}