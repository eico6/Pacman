#include "myheader.h"
#include <iostream>
#include <conio.h> // for getch()
#include <cstdio>  // random number generator
#include <cstdlib> // random number generator
#include <ctime>   // random number generator

void playGame(short int &queDirection, short int &curDirection, short int pacPosition[], short int lives,wchar_t board[][48],
              Enemy enemy[], int &score, int speedOfGame, bool &dead, short int &spawnedAi, short int &frameCounter, int &highScore)
{
    highScore = readHighScore(highScore);
    goToPositionInConsole(2, 24);
    std::wcout << "High-Score: " << highScore;
    printLivesLeft(lives);

    // This loops at the speedOfGame until pacman is completely dead (lives <= 0).
    // Techincally, pacman moves before the enemies, which MIGHT create undefined behaviour.
    // If this is a problem, put them into different threads.
    while (lives > 0) {
        if (dead == true){
            dead = false;
            resetPositions(enemy, spawnedAi, frameCounter, pacPosition, board);
            printLivesLeft(lives);
            pauseGame();
        }
        movePacMan(queDirection, curDirection, pacPosition, board);
        eatFood(pacPosition, score, board);
        ai(enemy, pacPosition, board, spawnedAi, frameCounter);
        printEntities(pacPosition, enemy , board);
        printScore(score);
        _sleep(speedOfGame);
        if (noMoreFood(board) == true){
            spawnFood(board);
            resetPositions(enemy, spawnedAi, frameCounter, pacPosition, board);
            pauseGame();
        }
        checkIfDead(dead, pacPosition, enemy);
        lives = (dead == true) ? lives -= 1 : lives;
    }
}

void resetAiPos(Enemy enemy[])
{
    for (int i{0}; i < 4; i++) {
        enemy[i].setPosition(23,11); // (23, 11) = center of board
        enemy[i].setJustSpawned(true);
    }
}

void eatFood(short int pacPosition[],  int &score, wchar_t board[][48])
{
    // because pacman is of size 2 cells in length, I have to check both cells for food
    if (board[pacPosition[1]][pacPosition[0]] == L'∙'){
        board[pacPosition[1]][pacPosition[0]] = L' ';
        score += 10;
    }
    if (board[pacPosition[1]][pacPosition[0] + 1] == L'∙'){
        board[pacPosition[1]][pacPosition[0] + 1] = L' ';
        score += 10;
    }

}

// Runs in the background to always get input from the player.
// The queDirection is changed, which is the player's desired direction.
// Pacman will change his currentDirection equal to queDirection as soon as possible.
void changeQueDirection (short int &queDirection, short int lives)
{
    char input{};

    while(lives > 0)
    {
        input = static_cast<char>(getch());

        switch (input)
        {
        case 72: // up arrow
            queDirection = UP;
            break;
        case 77: // right arrow
            queDirection = RIGHT;
            break;
        case 80: // down arrow
            queDirection = DOWN;
            break;
        case 75: // left arrow
            queDirection = LEFT;
            break;
        default:
            break;
        }
    }
}

void movePacMan(short int &queDirection, short int &curDirection, short int pacPosition[], wchar_t board[][48])
{
    int alignment = 0; // 0 = horizontal, 1 = vertical

    // This checks if queDirection is a valid curDirection.
    // If it is valid, pacman's current direction is changed to
    // queDirection. If not, he's gonna continue in his curDirection.
    if (queDirection == UP){
        if (!(board[pacPosition[1] - 1][pacPosition[0]] == L'█') &&
            !(board[pacPosition[1] - 1][pacPosition[0] + 1] == L'█')){
            curDirection = queDirection;
        }
    } else if (queDirection == RIGHT){
        if (!(board[pacPosition[1]][pacPosition[0] + 2] == L'█')){
            curDirection = queDirection;
        }
    } else if (queDirection == DOWN){
        if (!(board[pacPosition[1] + 1][pacPosition[0]] == L'█') &&
            !(board[pacPosition[1] + 1][pacPosition[0] + 1] == L'█')){
            curDirection = queDirection;
        }
    } else if (queDirection == LEFT){
        if (!(board[pacPosition[1]][pacPosition[0] - 1] == L'█')){
            curDirection = queDirection;
        }
    }

    // this for loop is responsible for pacman's movement
    for (int i{0}; i < 4; i++) {

        // this teleports pacman from one side of the map to the opposite side
        if (((pacPosition[0] == 46) && (pacPosition[1] == 11)) && (curDirection == RIGHT)){
            board[11][46] = L' '; // TEMP SOLUTION
            board[11][47] = L' '; // TEMP SOLUTION
            pacPosition[0] = 0;
            pacPosition[1] = 11;
            break;
        } else if (((pacPosition[0] == 0) && (pacPosition[1] == 11)) && (curDirection == LEFT)){
            pacPosition[0] = 46;
            pacPosition[1] = 11;
            break;
        }

        // this just moves pacman in his current direction
        if (curDirection == i){
            alignment = (i % 2 == 0) ? 1 : 0;
            if (alignment == 1){
                if (!(board[pacPosition[1] - 1 + i][pacPosition[0]] == L'█') &&
                    !(board[pacPosition[1] - 1 + i][pacPosition[0] + 1] == L'█')){
                        pacPosition[alignment] += (-1) + i;
                }
            } else {
                if (!(board[pacPosition[1]][pacPosition[0] + 3 - (static_cast<int>(i * 1.334))] == L'█')){
                    pacPosition[alignment] += 4 - (i * 2);
                }
            }
        }
    }
}

// this function calculates the quickest direction that will get the position
// of an AI, to be equal to the position of pacman.
void optimizeDirection(Enemy enemy[],  short int index, short int pacPosition[])
{
    short int horizontalValue{}; // if negative, ai moves LEFT
    short int verticalValue{};   // if negative, ai moves UP
    short int higherPriority{UP};
    short int lowerPriority{UP};

    // The difference between pacman and the AI in x-direction is stored in horizontalValue.
    // The difference between pacman and the AI in y-direction is stored in verticalValue.
    // The one with the highest absolute value of those two, will get the highest priority.
    // Higher absolute value = further away from pacman.
    if ((index == 0 || index == 3) && (enemy[index].getJustSpawned() == false)){
        horizontalValue = (pacPosition[0]) - (enemy[index].getPosX());
        verticalValue = (pacPosition[1]) - (enemy[index].getPosY());

        if (abs(horizontalValue) > abs(verticalValue)){
            higherPriority = (horizontalValue < 0) ? LEFT : RIGHT;
            lowerPriority = (verticalValue < 0) ? UP : DOWN;
        } else {
            higherPriority = (verticalValue < 0) ? UP : DOWN;
            lowerPriority = (horizontalValue < 0) ? LEFT : RIGHT;
        }
    } else if ((index == 1 || index == 2) && (enemy[index].getJustSpawned() == false)){
        higherPriority = randomDirection();
        lowerPriority = randomDirection();
    } // this "else if" statement makes ghost index 1 and 2 stupid with random directions

    enemy[index].setQueDirA(higherPriority);
    enemy[index].setQueDirB(lowerPriority);
}

// this function let's the AI move through gaps in the wall
// (won't be restricted to only move when they hit a wall)
void aiTakeShortcut(Enemy enemy[], short int index, wchar_t board[][48])
{
    // getQueDirA = most optimal direction
    if (enemy[index].getQueDirA() == UP){
        if (!(board[enemy[index].getPosY() - 1][enemy[index].getPosX()] == L'█') &&
            !(board[enemy[index].getPosY() - 1][enemy[index].getPosX() + 1] == L'█')){
            if (enemy[index].getDirection() != DOWN){ // AI can't change direction to the opposite one
                enemy[index].setDirection(enemy[index].getQueDirA());
            }
        }
    } else if (enemy[index].getQueDirA() == RIGHT){
        if (!(board[enemy[index].getPosY()][enemy[index].getPosX() + 2] == L'█')){
            if (enemy[index].getDirection() != LEFT){ // AI can't change direction to the opposite one
                enemy[index].setDirection(enemy[index].getQueDirA());
            }
        }
    } else if (enemy[index].getQueDirA() == DOWN){
        if (!(board[enemy[index].getPosY() + 1][enemy[index].getPosX()] == L'█') &&
            !(board[enemy[index].getPosY() + 1][enemy[index].getPosX() + 1] == L'█')){
            if (enemy[index].getDirection() != UP){ // AI can't change direction to the opposite one
                enemy[index].setDirection(enemy[index].getQueDirA());
            }
         }
    } else if (enemy[index].getQueDirA() == LEFT){
        if (!(board[enemy[index].getPosY()][enemy[index].getPosX() - 1] == L'█')){
            if (enemy[index].getDirection() != RIGHT){ // AI can't change direction to the opposite one
                enemy[index].setDirection(enemy[index].getQueDirA());
            }
        }
    }
}

void aiChangeDirection(Enemy enemy[], short int index, bool &alreadyChanged)
{
    if (alreadyChanged == false){
        enemy[index].setDirection(enemy[index].getQueDirB());
        alreadyChanged = true;
    } else {
        enemy[index].setDirection(randomDirection());
    }
}

// Everything about the AI's logic is stored inside this function.
void ai(Enemy enemy[], short int pacPosition[], wchar_t board[][48], short int &spawnedAi, short int &frameCounter)
{
    bool alreadyChanged = false;
    bool accepted = false;

    for (short int i{0}; i < spawnedAi; i++) {
        alreadyChanged = false; // explanation of this variable is found below
        accepted = false;       // if an AI's direction is accepted to be valid

        optimizeDirection(enemy, i, pacPosition);
        aiTakeShortcut(enemy, i, board);

        // This while loop filters out the AI's direction to be valid (should not walk through walls).
        // First direction to test for validation, is the highest priority one.
        // If that direction fails the test (there is a wall blocking the passage),
        // alreadyChanged will be equal to true, and the next direction for testing is the lower priority one.
        // If that one fails aswell, the AI will be assigned a random direction until valid
        // (because alreadyChanged is equal to true). So the boolean alreadyChanged keeps track
        // of which direction the AI should be assigned. If true, assign a random one.
        while (accepted == false) {
            if (enemy[i].getDirection() == UP){
                if ((board[enemy[i].getPosY() - 1][enemy[i].getPosX()] == L'█') &&
                    (board[enemy[i].getPosY() - 1][enemy[i].getPosX() + 1]) == L'█')
                {
                    aiChangeDirection(enemy, i, alreadyChanged);
                } else {
                    accepted = true;
                }
            } else if (enemy[i].getDirection() == RIGHT){
                if (board[enemy[i].getPosY()][enemy[i].getPosX() + 2] == L'█')
                {
                    aiChangeDirection(enemy, i, alreadyChanged);
                } else {
                    accepted = true;
                }
            } else if (enemy[i].getDirection() == DOWN){
                if ((board[enemy[i].getPosY() + 1][enemy[i].getPosX()] == L'█') &&
                    (board[enemy[i].getPosY() + 1][enemy[i].getPosX() + 1]) == L'█')
                {
                    aiChangeDirection(enemy, i, alreadyChanged);
                } else {
                    accepted = true;
                }
            } else if (enemy[i].getDirection() == LEFT){
                if (board[enemy[i].getPosY()][enemy[i].getPosX() - 1] == L'█')
                {
                    aiChangeDirection(enemy, i, alreadyChanged);
                } else {
                    accepted = true;
                }
            }
        }

        // direction is now filtered out to be valid, so next is to move AI
        if (enemy[i].getDirection() == UP){
            enemy[i].setPosition(enemy[i].getPosX(), enemy[i].getPosY() - 1);
        } else if (enemy[i].getDirection() == RIGHT){
            enemy[i].setPosition(enemy[i].getPosX() + 1, enemy[i].getPosY());
        } else if (enemy[i].getDirection() == DOWN){
            enemy[i].setPosition(enemy[i].getPosX(), enemy[i].getPosY() + 1);
        } else if (enemy[i].getDirection() == LEFT){
            enemy[i].setPosition(enemy[i].getPosX() - 1, enemy[i].getPosY());
        }

        // frames before next ai starts moving
        if (frameCounter >= 10){
            spawnedAi ++;
            frameCounter = 0;
        }
    }

    // if frameCounter < 4, is to make sure the last ghost is not stuck in "justSpawned" mode
    if (spawnedAi < 4 || frameCounter < 4){
        frameCounter ++;
    }

    // the most optimized direction for the first 3 frames of each AI's lifetime is always 'UP'
    if (frameCounter == 3){
        for (int i{0}; i < spawnedAi; i++) {
            enemy[i].setJustSpawned(false);
        }
    }

}

short int randomDirection()
{
    short int rng{};
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec);
    rng = (rand() % 4);

    return rng;
}
