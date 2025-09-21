#include "NameInputScreen.h"
#include <iostream>

NameInputScreen::NameInputScreen(sf::RenderWindow& window, sf::Font& font) 
    : window(window), font(font), playerName(""), m_isActive(false) {
    
    titleText.setFont(font);
    titleText.setString("ENTER YOUR NAME");
    titleText.setCharacterSize(50);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 100);
    
    promptText.setFont(font);
    promptText.setString("Type your name and press ENTER:");
    promptText.setCharacterSize(30);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(window.getSize().x / 2 - promptText.getLocalBounds().width / 2, 180);
    
    startText.setFont(font);
    startText.setString("Press ENTER to start or Button A on controller");
    startText.setCharacterSize(25);
    startText.setFillColor(sf::Color::Yellow);
    startText.setPosition(window.getSize().x / 2 - startText.getLocalBounds().width / 2, 350);
    
    inputText.setFont(font);
    inputText.setCharacterSize(40);
    inputText.setFillColor(sf::Color::Yellow);
}

void NameInputScreen::activate() {
    m_isActive = true;
    playerName = "";
    inputText.setString("_");
    inputText.setPosition(window.getSize().x / 2 - 10, 250);
}

void NameInputScreen::deactivate() {
    m_isActive = false;
}

bool NameInputScreen::isActive() const {
    return m_isActive;
}

std::string NameInputScreen::getPlayerName() const {
    return playerName;
}

void NameInputScreen::handleEvent(sf::Event& event) {
    if (!isActive()) return;
    
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') { // Backspace
            if (!playerName.empty()) {
                playerName.pop_back();
            }
        } else if (event.text.unicode == '\r') { // Enter
            if (!playerName.empty()) {
                deactivate();
            }
        } else if (event.text.unicode < 128 && playerName.length() < 15) {
            playerName += static_cast<char>(event.text.unicode);
        }
        
        // Atualiza o texto de entrada
        inputText.setString(playerName + "_");
        inputText.setPosition(window.getSize().x / 2 - inputText.getLocalBounds().width / 2, 250);
    }
    // Controle - Botão A (0) para confirmar
    else if (event.type == sf::Event::JoystickButtonPressed && 
             event.joystickButton.button == 0) {
        if (!playerName.empty()) {
            deactivate();
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