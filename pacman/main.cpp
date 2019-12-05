#include "myheader.h"
#include <iostream>
#include <fcntl.h> // for unicode characters
#include <io.h>    // for unicode characters
#include <thread>

// I've decided to use unicode characters for a more aesthetically pleasing experience.
// Instead of using the typical 'char' data type, 'wchar_t' is used instead.
// Same goes for outputs, from 'cout' to 'wcout'.

int main()
{
    Enemy enemy[4] = {};
    bool dead = false;
    int highScore{0};
    int score{0};
    int speedOfGame{100};                   // milliseconds before next frame, lower is quicker gameplay
    short int queDirection{LEFT};           // player's desired direction
    short int curDirection{LEFT};           // pacman's current/valid direction
    short int spawnedAi{1};                 // how many enemies should start to move first frame
    short int frameCounter{0};              // used in AI's logic
    short int pacPosition [2] = {24, 19};
    short int lives{3};

    _setmode(_fileno(stdout), _O_U16TEXT);  // this is required for the use of unicode characters
    wchar_t w = L'█';                       // wall
    wchar_t d = L'▀';                       // door
    wchar_t _ = ' ';                        // space
    wchar_t board[23][48] = {
                            {w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w},
                            {w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w},
                            {w,w,_,_,w,w,w,w,w,w,_,_,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,_,_,w,w,w,w,w,w,_,_,w,w},
                            {w,w,_,_,w,w,w,w,w,w,_,_,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,_,_,w,w,w,w,w,w,_,_,w,w},
                            {w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w},
                            {w,w,_,_,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,_,_,w,w},
                            {w,w,_,_,w,w,w,w,w,w,_,_,w,w,w,w,_,_,_,_,_,_,w,w,w,w,_,_,_,_,_,_,w,w,w,w,_,_,w,w,w,w,w,w,_,_,w,w},
                            {w,w,_,_,_,_,_,_,_,_,_,_,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,_,_,_,_,_,_,_,_,_,_,w,w},
                            {w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w},
                            {w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,d,d,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w},
                            {w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,_,_,_,_,_,_,_,_,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w},
                            {_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,_,_,_,_,_,_,_,_,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_},
                            {w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,_,_,_,_,_,_,_,_,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w},
                            {w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w},
                            {w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w},
                            {w,w,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,w,w,w,w,w,w,w,w,w,w,w,w,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,w,w},
                            {w,w,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,w,w},
                            {w,w,_,_,_,_,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,_,_,_,_,w,w},
                            {w,w,w,w,_,_,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,_,_,w,w,w,w},
                            {w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w,w,w,_,_,_,_,_,_,_,_,w,w,w,w,_,_,_,_,_,_,_,_,_,_,_,_,_,_,w,w},
                            {w,w,_,_,w,w,w,w,w,w,_,_,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,w,w,_,_,w,w,_,_,w,w,w,w,w,w,_,_,w,w},
                            {w,w,_,_,_,_,_,_,_,_,_,_,w,w,_,_,_,_,_,_,_,_,w,w,w,w,_,_,_,_,_,_,_,_,w,w,_,_,_,_,_,_,_,_,_,_,w,w},
                            {w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w,w},
                            };

    std::thread pacDirections(changeQueDirection, std::ref(queDirection), lives);
    playGame(queDirection, curDirection, pacPosition, lives, board, enemy,
             score, speedOfGame, dead, spawnedAi, frameCounter, highScore);

    // pacman is now dead
    system("CLS");
    printScoreboard(score, highScore);
    goToPositionInConsole(8, 10);
    pacDirections.detach();
    system("PAUSE");
    return 0;
}


