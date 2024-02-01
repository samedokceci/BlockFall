#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game); // Function that implements the gameplay
private:
    void printPowerUp(BlockFall& game);
    static void printControls();
    void gameEnded(BlockFall& game);

    bool initiateBlock(BlockFall& game);
    void removeBlockFromGrid(BlockFall& game, Block* block);
    int moveLeft(BlockFall& game); 
    int moveRight(BlockFall& game); 
    void printGrid(BlockFall& game);
    int rotateLeft(BlockFall& game);
    int rotateRight(BlockFall& game);
    int drop(BlockFall& game);
    void applyGravity(BlockFall& game);
    void freeTheGrid(BlockFall& game);
    bool checkPowerUp(BlockFall& game);
    void gameOver(BlockFall& game);
    static void cleanGrid(BlockFall& game);
    void writeBlockToGrid(BlockFall& game, unsigned long rowOfGrid);
    void dropTheBlock(BlockFall& game);
    bool checkToFree(BlockFall& game);
    void freeRow(BlockFall& game,int row);
    int countOccupiedCells(int rows, int cols, vector<vector<bool>> shape);
};



#endif //PA2_GAMECONTROLLER_H
