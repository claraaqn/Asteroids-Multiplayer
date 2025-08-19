#include "Menu.h"

Menu::Menu(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font), selectedItemIndex(0) {

    //? Configura o título
    titleText.setFont(font);
    titleText.setString("ASTEROIDS"); // Título mais genérico agora
    titleText.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 10));
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
    titleText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 4.0f);

    //? Configura os créditos
    creditsText.setFont(font);
    creditsText.setString("MENINAS NA CIENCIA APRESENTA");
    creditsText.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 30));
    creditsText.setFillColor(sf::Color::White);
    creditsText.setOrigin(creditsText.getLocalBounds().width / 2, creditsText.getLocalBounds().height / 2);
    creditsText.setPosition(window.getSize().x / 2.0f, window.getSize().y - 50.0f);

    //? Configura os itens do menu
    std::vector<std::string> options = {"SINGLE PLAYER", "MULTIPLAYER", "EXIT"};
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(options[i]);
        text.setCharacterSize(static_cast<unsigned int>(GameConstants::HEIGHT / 20));
        text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
        // Posiciona os itens um abaixo do outro
        text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + i * 60.0f);
        menuItems.push_back(text);
    }
    
    // Atualiza a cor do primeiro item selecionado
    updateMenuText();
}

void Menu::draw() {
    window.setView(window.getDefaultView());
    window.clear(sf::Color::Black);
    window.draw(titleText);
    window.draw(creditsText);
    for (const auto& item : menuItems) {
        window.draw(item);
    }
    window.display();
}

GameMode Menu::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
                std::cout << "Tecla Pressionada! Codigo: " << event.key.code << std::endl;

        switch (event.key.code) {
            case sf::Keyboard::Up:
                moveUp();
                break;
            case sf::Keyboard::Down:
                moveDown();
                break;
            
            case sf::Keyboard::Return: 
                std::cout << "Enter pressionado! Item selecionado: " << selectedItemIndex << std::endl;
                if (selectedItemIndex == 0) {
                    return GameMode::SinglePlayer;
                }
                if (selectedItemIndex == 1) {
                    return GameMode::Multiplayer;
                }
                if (selectedItemIndex == 2) {
                    return GameMode::Exit;
                }
                break;
            case sf::Keyboard::Q:
                return GameMode::Exit;
                break;
            default:
                break;
        }
    }
    return GameMode::None; 
}

void Menu::updateMenuText(){
    for (size_t i = 0; i < menuItems.size(); ++i){
        if (i == selectedItemIndex)
        {
            menuItems[i].setFillColor(sf::Color::Green);

        }else{
            menuItems[i].setFillColor(sf::Color::White);
        }
        
    }
}

void Menu::moveUp(){
     if (selectedItemIndex - 1 >= 0) {
        selectedItemIndex--;
    } else {
        selectedItemIndex = menuItems.size() - 1; // Volta para o último item
    }
    updateMenuText();
}

void Menu::moveDown() {
    if (selectedItemIndex + 1 < menuItems.size()) {
        selectedItemIndex++;
    } else {
        selectedItemIndex = 0; // Volta para o primeiro item
    }
    updateMenuText();
}