# README - Jogo SBPC: Asteroids Multiplayer

## 📌 Sobre o Projeto
*Jogo SBPC* é um jogo de Asteroids multiplayer desenvolvido como parte do projeto de extensão Meninas na Ciência, com o objetivo de:
- Demonstrar aplicações práticas do curso de Engenharia da Computação
- Inspirar jovens (especialmente meninas) a seguirem carreiras em STEM
- Ser apresentado em congressos científicos como a SBPC (Sociedade Brasileira para o Progresso da Ciência)

## 👩‍💻 Equipe
- [Clara Aquino](https://github.com/claraaqn) - Desenvolvedora
- [Clara Ferreira](https://github.com/MClaraFerreira5) - Desenvolvedora
- Orientação: Milene Figueira

## 🎮 Sobre o Jogo
- *Gênero*: Ação/Arcade
- *Estilo*: Inspirado no clássico Asteroids
- *Diferenciais*:
  - Modo multiplayer local (2 jogadores)
  - Controles por teclado e joystick
  - Sistema de física e partículas
  - Efeitos sonoros e visuais

## 🛠 Tecnologias Utilizadas
- Linguagem: C++17
- Biblioteca gráfica: SFML 2.5+
- Ferramentas:
  - Visual Studio Code
  - Git/GitHub
  - Compilador MinGW

## 📂 Estrutura do Projeto

jogo-asteroids/
├── assets/
│   ├── font/                  # Fontes do jogo
│   │   ├── PixelifySans-*.ttf
│   ├── imgs/                  # Imagens
│   │   └── Fundo_Tela2.jpg
│   └── sound/                 # Efeitos sonoros
│       ├── explosion.wav
│       └── laser1.wav
├── include/                   # Headers
│   ├── Asteroid.h
│   ├── AsteroidExplosion.h
│   ├── Bullet.h
│   ├── Game.h
│   ├── GameConstants.h
│   ├── GameOverScreen.h
│   ├── Menu.h
│   ├── PerformanceTracker.h
│   ├── Spaceship.h
│   └── Starfield.h
├── src/                       # Implementações
│   ├── Asteroid.cpp
│   ├── Bullet.cpp
│   ├── Game.cpp
│   ├── GameOverScreen.cpp
│   ├── Menu.cpp
│   ├── PerformanceTracker.cpp
│   ├── Spaceship.cpp
│   └── Starfield.cpp
├── main.cpp                   # Ponto de entrada
├── .gitignore
└── jogo.exe                   # Executável


## 🚀 Como Executar
1. *Pré-requisitos*:
   - Windows 7+
   - Arquivos DLL do SFML (incluídos no pacote)

2. *Execução*:
   bash
   # Clone o repositório
   git clone [[URL do repositório]](https://github.com/claraaqn/Asteroids-Multiplayer)
   
   # Execute o jogo
   ./jogo.exe
   

3. *Controles*:
   - *Jogador 1*: Teclado (WASD para movimento, Espaço para atirar)
   - *Jogador 2*: Joystick ou teclado alternativo

## 🌟 Objetivos Educacionais
Este projeto demonstra conceitos de:
- Programação orientada a objetos
- Matemática aplicada (vetores, física)
- Gerenciamento de recursos
- Lógica de jogos
- Trabalho em equipe

## 📅 Roadmap
- [ ] Adicionar menu de configurações
- [ ] Implementar sistema de pontuação online
- [ ] Criar modos de dificuldade
- [ ] Desenvolver versão web (via Emscripten)

## 📜 Licença
Este projeto está licenciado sob a MIT License - veja o arquivo [LICENSE.md] para detalhes.

## 🙌 Agradecimentos
- À [Instituição de Ensino] pelo apoio
- Ao programa Meninas na Ciência
- Aos professores orientadores

---

"Inspirando a próxima geração de mulheres na computação através da criação de jogos!" 🚀👩‍💻
