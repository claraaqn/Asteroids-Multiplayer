// GameOverScreen.cpp
#include "GameOverScreen.h"
#include <sstream>
#include <iomanip>

GameOverScreen::GameOverScreen(const sf::Font& font, HighScoreDB& highScoreDB) 
    : highScoreDB(highScoreDB) {
    
    // Configuração dos textos
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER!");
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(sf::Color::White);
    
    winnerText.setFont(font);
    winnerText.setCharacterSize(30);
    winnerText.setFillColor(sf::Color::White);
    
    restartText.setFont(font);
    restartText.setString("PRESS 'R' TO RESTART");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);

    quitText.setFont(font);
    quitText.setString("PRESS 'Q' TO QUIT");
    quitText.setCharacterSize(30);
    quitText.setFillColor(sf::Color::White);
    
    highScoresTitle.setFont(font);
    highScoresTitle.setString("TOP 5 SCORES:");
    highScoresTitle.setCharacterSize(25);
    highScoresTitle.setFillColor(sf::Color::Yellow);
    
    // Inicializa textos dos highscores
    for (int i = 0; i < 5; ++i) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        highScoreTexts.push_back(text);
    }
    
    // Centraliza todos os textos horizontalmente
    centerText(gameOverText);
    centerText(winnerText);
    centerText(restartText);
    centerText(quitText);
    centerText(highScoresTitle);

    // Posicionamento vertical inicial (será ajustado no setPosition)
    gameOverText.setPosition(0, 0);
    winnerText.setPosition(0, 80);
    highScoresTitle.setPosition(0, 140);
    
    for (size_t i = 0; i < highScoreTexts.size(); ++i) {
        highScoreTexts[i].setPosition(0, 180 + i * 30);
    }
    
    restartText.setPosition(0, 340);
    quitText.setPosition(0, 380);
}

void GameOverScreen::update(int winner, int score1, int score2, GameMode gameMode) {
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
    refreshHighScores(gameMode);
}

void GameOverScreen::refreshHighScores(GameMode mode) {
    std::vector<HighScore> topScores = highScoreDB.getTopScores(5);
    updateHighScoresDisplay();
}

void GameOverScreen::updateHighScoresDisplay() {
    std::vector<HighScore> topScores = highScoreDB.getTopScores(5);
    
    for (size_t i = 0; i < highScoreTexts.size(); ++i) {
        if (i < topScores.size()) {
            std::stringstream ss;
            ss << (i + 1) << ". " << topScores[i].playerName << " - " 
               << topScores[i].score << " - " 
               << (topScores[i].gameMode == GameMode::SinglePlayer ? "Single" : "Multi");
            highScoreTexts[i].setString(ss.str());
        } else {
            highScoreTexts[i].setString("");
        }
        centerText(highScoreTexts[i]);
    }
}

void GameOverScreen::draw(sf::RenderWindow& window) {
    window.draw(gameOverText);
    window.draw(winnerText);
    window.draw(highScoresTitle);
    
    for (auto& scoreText : highScoreTexts) {
        window.draw(scoreText);
    }
    
    window.draw(restartText);
    window.draw(quitText);
}

void GameOverScreen::setPosition(float x, float y) {
    // Reposiciona todos os elementos relativos ao centro da tela
    gameOverText.setPosition(x, y - 200);
    winnerText.setPosition(x, y - 120);
    highScoresTitle.setPosition(x, y - 60);
    
    for (size_t i = 0; i < highScoreTexts.size(); ++i) {
        highScoreTexts[i].setPosition(x, y - 20 + i * 25);
    }
    
    restartText.setPosition(x, y + 120);
    quitText.setPosition(x, y + 160);
    
    // Recentraliza todos os textos
    centerText(gameOverText);
    centerText(winnerText);
    centerText(highScoresTitle);
    for (auto& scoreText : highScoreTexts) {
        centerText(scoreText);
    }
    centerText(restartText);
    centerText(quitText);
}

void GameOverScreen::centerText(sf::Text& text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2, bounds.height / 2);
}