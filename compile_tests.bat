@echo off
echo Compilando todos os testes corretamente...

g++ tests/catch_amalgamated.cpp ^
    tests/test_main.cpp ^
    tests/test_asteroid.cpp ^
    tests/test_bullet.cpp ^
    tests/test_game.cpp ^
    tests/test_gameoverscreen.cpp ^
    tests/test_highscoredb.cpp ^
    tests/test_integration.cpp ^
    tests/test_menu.cpp ^
    tests/test_nameinputscreen.cpp ^
    tests/test_performancetracker.cpp ^
    tests/test_spaceship.cpp ^
    tests/test_starfield.cpp ^
    tests/test_supershot.cpp ^
    src/Asteroid.cpp ^
    src/Bullet.cpp ^
    src/Game.cpp ^
    src/GameOverScreen.cpp ^
    src/HighScoreDB.cpp ^
    src/Menu.cpp ^
    src/NameInputScreen.cpp ^
    src/PerformanceTracker.cpp ^
    src/Spaceship.cpp ^
    src/Starfield.cpp ^
    src/SuperShot.cpp ^
    -Iinclude ^
    -I"C:\SFML-2.6.1-windows-ucrt64-mingw\include" ^
    -L"C:\SFML-2.6.1-windows-ucrt64-mingw\lib" ^
    -lsfml-graphics ^
    -lsfml-window ^
    -lsfml-system ^
    -lsqlite3 ^
    -std=c++17 ^
    -Wall ^
    -o test_runner.exe

if %errorlevel% equ 0 (
    echo Compilacao bem-sucedida!
    echo Executando testes...
    test_runner.exe
    pause
) else (
    echo Erro na compilacao!
    pause
)