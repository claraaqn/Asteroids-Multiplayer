#include "NameInputScreen.h"
#include <iostream>

NameInputScreen::NameInputScreen(sf::RenderWindow& window, sf::Font& font) 
    : window(window), font(font), currentPlayer(1), m_isActive(false) {
    
    titleText.setFont(font);
    titleText.setString("ENTER YOUR NAME");
    titleText.setCharacterSize(50);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 100);
    
    promptText.setFont(font);
    promptText.setString("Player 1, type your name and press ENTER:");
    promptText.setCharacterSize(30);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(window.getSize().x / 2 - promptText.getLocalBounds().width / 2, 180);
    
    startText.setFont(font);
    startText.setString("Press ENTER to confirm or Button A on controller");
    startText.setCharacterSize(25);
    startText.setFillColor(sf::Color::Yellow);
    startText.setPosition(window.getSize().x / 2 - startText.getLocalBounds().width / 2, 350);
    
    inputText.setFont(font);
    inputText.setCharacterSize(40);
    inputText.setFillColor(sf::Color::Yellow);
}

void NameInputScreen::activate(GameMode mode) {
    m_isActive = true;
    player1Name = "";
    player2Name = "";
    currentPlayer = 1;

    gameMode = mode;

    if (gameMode == GameMode::SinglePlayer) {
        promptText.setString("Type your name and press ENTER:");
    } else if(gameMode == GameMode::Multiplayer) {
        promptText.setString("Player 1, type your name and press ENTER:");
    }

    inputText.setString("_");
    inputText.setPosition(window.getSize().x / 2 - 10, 250);
}

void NameInputScreen::deactivate() {
    m_isActive = false;
}

bool NameInputScreen::isActive() const {
    return m_isActive;
}

std::string NameInputScreen::getPlayer1Name() const {
    return player1Name;
}

std::string NameInputScreen::getPlayer2Name() const {
    return player2Name;
}

void NameInputScreen::handleEvent(sf::Event& event) {
    if (!isActive()) return;

    std::string& currentName = (currentPlayer == 1 ? player1Name : player2Name);

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') {
            if (!currentName.empty()) {
                currentName.pop_back();
            }
        } else if (event.text.unicode == '\r') {
            if (!currentName.empty()) {
                if (gameMode == GameMode::Multiplayer && currentPlayer == 1) {
                    currentPlayer = 2;
                    promptText.setString("Player 2, type your name and press ENTER:");
                    currentName = "";
                } else {
                    deactivate();
                }
            }
        } else if (event.text.unicode < 128 && currentName.length() < 15) {
            currentName += static_cast<char>(event.text.unicode);
        }
        
        inputText.setString(currentName + "_");
        inputText.setPosition(window.getSize().x / 2 - inputText.getLocalBounds().width / 2, 250);
    }
    else if (event.type == sf::Event::JoystickButtonPressed && 
             event.joystickButton.button == 0) {
        if (!currentName.empty()) {
            if (currentPlayer == 1) {
                currentPlayer = 2;
                promptText.setString("Player 2, type your name and press A:");
                currentName = "";
            } else {
                deactivate();
            }
        }
    }
}

void NameInputScreen::draw() {
    if (!isActive()) return;
    
    window.draw(titleText);
    window.draw(promptText);
    window.draw(inputText);
    window.draw(startText);
}
