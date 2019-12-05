#include "myheader.h"
#include <iostream>
#include <io.h>      // for unicode characters
#include <fcntl.h>   // for unicode characters
#include <windows.h> // for SetConsoleCursorPosition
#include <fstream>   // file manipulation

void saveHighScore(int highScore)
{
    std::ofstream cacheFile;
    cacheFile.open("highScore.txt");
    cacheFile << highScore;
    cacheFile.close();
}

int readHighScore(int highScore)
{
    std::ifstream cacheFile;
    cacheFile.open("highScore.txt");
    cacheFile >> highScore;
    cacheFile.close();

    return highScore;
}

bool noMoreFood(wchar_t board[][48])
{
    bool empty = true;

    // scans through the whole board for food
    for (short int i{0}; i < 23; i++) {
        for (short int j{0}; j < 48; j++) {
            if (board[i][j] == L'∙'){
                empty = false;
                break;
            }
        }
    }

    return empty;
}

void resetPositions(Enemy enemy[], short int &spawnedAi, short int &frameCounter, short int pacPosition[], wchar_t board[][48])
{
    pacPosition[0] = 24; // puts pacman back to his spawn point
    pacPosition[1] = 19; // puts pacman back to his spawn point
    resetAiPos(enemy);
    spawnedAi = 1;
    frameCounter = 0;
    printStaticBoard(board);
    printEntities(pacPosition, enemy, board);
}

void printScore(int score)
{
    goToPositionInConsole(22, 24);
    std::wcout << "Score: " << score;
}

void printLivesLeft(short int lives){
    goToPositionInConsole(38, 24);
    std::wcout << "Lives: " << lives;
}

void spawnFood(wchar_t board[][48])
{
    wchar_t food = L'∙';

    // scans the board and spawns food at empty cells
    for (short int i{0}; i < 23; i++) {
        for (short int j{0}; j < 48; j++) {
            // won't spawn food inside the enemy spawn point
            if (board[i][j] == L' ' && !((i < 13 && i > 9) && (j < 28 && j > 19))){
                board[i][j] = food;
            }
        }
    }
}

// prints the level itself (empty)
void printStaticBoard(wchar_t board[][48])
{
    goToPositionInConsole(0, 0);
    for (short int y{0}; y < 23; y ++) {
        goToPositionInConsole(0, y);
        for (short int x{0}; x < 48; x ++) {
            std::wcout << board[y][x];
        }
    }
}

void printEntities(short int pacPosition[], Enemy enemy[], wchar_t board[][48])
{
    bool enemySpotted = false;
    short int line{};

    // scans the board vertically to find enemies/pacman who should be printed out every frame
    for (short int i{0}; i < 23; i++) {
        enemySpotted = false;
        for (short int j{0}; j < 4; j++) { // 4 = total amount of enemies
            if ((enemy[j].getPosY() - 1) == i){
                enemySpotted = true;
                break;
            }
        }

        // if pacman or an enemy is detected, print that line
        if ((pacPosition[1] - 1 == i) || (enemySpotted == true)){
            line = i;
            printLines(pacPosition, line, enemy, board);
        }
    }
}

void printLines(short int pacPosition[], short int line, Enemy enemy[], wchar_t board[][48])
{
    bool enemySpotted = false;
    wchar_t p = L'☻'; // player
    wchar_t e = L'◘'; // enemy

    // This scans horizontally on the line where printEnteties() detected an entity.
    // Checks if the entity who was detected was pacman or an enemy.
    for (short int k{line}; k < (line + 3); k++) {
        goToPositionInConsole(0, k);
        for (short int i{0}; i < 48; i++) {
            enemySpotted = false;
            for (short int m{0}; m < 4; m++) {
                if ((enemy[m].getPosX() == i || enemy[m].getPosX() == i - 1) && (enemy[m].getPosY() == k)){
                    enemySpotted = true;
                    break;
                }
            }
            if (enemySpotted == true){
                std::wcout << e << e;
                i++;
            } else if ((i == pacPosition[0]) && (k == pacPosition[1])){
                std::wcout << p << p;
                i++;
            } else {
                std::wcout << board[k][i];
            }
        }
    }
}

void checkIfDead(bool &dead, short int pacPosition[], Enemy enemy[])
{
    // I choose to increase the hitbox of the enemy by one cell upwards and downwards.
    // I figured after some testing, that this would result in a more accurate solution.
    for (int i{0}; i < 4; i++) {
        if (((pacPosition[0] == enemy[i].getPosX()) ||
            (pacPosition[0]) == (enemy[i].getPosX() - 1) ||
            (pacPosition[0]) == (enemy[i].getPosX() + 1)) &&
            ((pacPosition[1] == enemy[i].getPosY()) ||
            (pacPosition[1] == enemy[i].getPosY() - 1) ||
            (pacPosition[1] == enemy[i].getPosY() + 1))){
            dead = true;
            break;
        }
    }
}

// instead of printing out the whole board each frame, I only print out the
// necessary lines for optimal performance
void goToPositionInConsole(short x, short y)
{
    toggleCursorVisibility(false);
    COORD coordinates;
    coordinates.X = x;
    coordinates.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
}

// disables the little "blinking dot", only for visualization
void toggleCursorVisibility(bool showCursor)
{
    HANDLE outputHandle=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    cursorInfo.dwSize=1;
    GetConsoleCursorInfo(outputHandle, &cursorInfo);
    cursorInfo.bVisible=showCursor;
    SetConsoleCursorInfo(outputHandle, &cursorInfo);
}

// the same as system("PAUSE"), but because of the use of SetConsoleCursorPosition,
// I have to manually decide where to write the "Press any key to continue..."
void pauseGame(){
    goToPositionInConsole(10, 27);
    system("PAUSE");

    // this overwrites the text with whitespace (works as an eraser)
    goToPositionInConsole(10, 27);
    std::wcout << "                               ";
}

void printScoreboard(int score, int highScore){
    wchar_t v = L'║'; // vertical
    wchar_t h = L'═'; // horizontal
    wchar_t u = L'╔'; // leftUpCorner
    wchar_t i = L'╗'; // rightUpCorner
    wchar_t o = L'╝'; // rightDownCorner
    wchar_t p = L'╚'; // leftDownCorner

    goToPositionInConsole(8, 3);
    std::wcout << u << h << h << h << h << h << h << h << h << h << h << h
               << h << h << h << h << h << h << h << h << h << h << h << h << i;
    goToPositionInConsole(8, 4);
    std::wcout << v << "                       " << v;
    goToPositionInConsole(8, 5);
    std::wcout << v << "                       " << v;
    goToPositionInConsole(8, 6);
    std::wcout << v << "                       " << v;
    goToPositionInConsole(8, 7);
    std::wcout << p << h << h << h << h << h << h << h << h << h << h << h
               << h << h << h << h << h << h << h << h << h << h << h << h << o;

    if (readHighScore(highScore) < score){
        goToPositionInConsole(13, 3);
        std::wcout << "NEW HIGHSCORE!";
        goToPositionInConsole(9, 5);
        std::wcout << "Old Highscore: " << highScore;
        highScore = score;
        goToPositionInConsole(9, 6);
        std::wcout << "New Highscore: " << highScore;
        saveHighScore(score);
    } else {
        goToPositionInConsole(15, 3);
        std::wcout << "SCOREBOARD";
        goToPositionInConsole(13, 5);
        std::wcout << "Score: " << score;
        goToPositionInConsole(9, 6);
        std::wcout << "Highscore: " << highScore;
    }
}
