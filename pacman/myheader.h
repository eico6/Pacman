#ifndef MYHEADER_H
#define MYHEADER_H

enum Directions{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

class Enemy
{
private:
    bool justSpawned = true;
    short int enemyPosition[2] = {};
    short int direction = UP;
    short int queDirA{UP}; // most optimized direction
    short int queDirB{UP}; // second most optimized direction
public:
    void setPosition(short int inputX, short int inputY){
    enemyPosition[0] = inputX;
    enemyPosition[1] = inputY;
    }
    void setDirection(short int input){ direction = input; }
    void setQueDirA(short int input){ queDirA = input; }
    void setQueDirB(short int input){ queDirB = input; }
    void setJustSpawned(bool input){ justSpawned = input; }
    bool getJustSpawned(){ return justSpawned; }
    short int getPosX(){ return enemyPosition[0]; }
    short int getPosY(){ return enemyPosition[1]; }
    short int getDirection(){ return direction; }
    short int getQueDirA(){ return queDirA; }
    short int getQueDirB(){ return queDirB; }
};


// MOVEMENT
void playGame(short int &queDirection, short int &curDirection, short int pacPosition[], short int lives,wchar_t board[][48],
              Enemy enemy[], int &score, int speedOfGame, bool &dead, short int &spawnedAi, short int &frameCounter, int &highScore);
void resetAiPos(Enemy enemy[]);
void eatFood(short int pacPosition[],  int &score, wchar_t board[][48]);
void changeQueDirection(short int &queDirection, short int lives);
void movePacMan(short int &queDirection, short int &curDirection, short int pacPosition[], wchar_t board[][48]);
void optimizeDirection(Enemy enemy[], short int index, short int pacPosition[]);
void aiTakeShortcut(Enemy enemy[], short int index, wchar_t board[][48]);
void aiChangeDirection(Enemy enemy[], short int index, bool &alreadyChanged);
void ai(Enemy enemy[], short int pacPosition[], wchar_t board[][48], short int &spawnedAi, short int &frameCounter);
short int randomDirection();

// PRINTING
void saveHighScore(int highScore);
int readHighScore(int highScore);
bool noMoreFood(wchar_t board[][48]);
void resetPositions(Enemy enemy[], short int &spawnedAi, short int &frameCounter, short int pacPosition[], wchar_t board[][48]);
void printScore(int score);
void printLivesLeft(short int lives);
void spawnFood(wchar_t board[][48]);
void printStaticBoard(wchar_t board[][48]);
void printEntities(short int pacPosition[], Enemy enemy[], wchar_t board[][48]);
void printLines(short int pacPosition[], short int line, Enemy enemy[], wchar_t board[][48]);
void checkIfDead(bool &dead, short int pacPosition[], Enemy enemy[]);
void goToPositionInConsole(short x, short y);
void toggleCursorVisibility(bool showCursor);
void pauseGame();
void printScoreboard(int score, int highScore);

#endif // MYHEADER_H
