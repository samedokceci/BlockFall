#include "GameController.h"
#include <ncurses.h>

#define KEY_MOVE_RIGHT 100 //d KEY
#define KEY_MOVE_LEFT 97 //a KEY
#define KEY_ROT_RIGHT 101 //e KEY
#define KEY_ROT_LEFT  113 //q KEY
#define KEY_DROP 115 //s KEY
#define KEY_FINISH 111 //o KEY
#define KEY_GRAVITY 103 //g KEY

bool GameController::play(BlockFall& game){
    int cValue = 150;
    initiateBlock(game);
    printControls();
    printGrid(game);
    char command;
    while(cValue!=KEY_EXIT) {
        clear();
        printControls();
        printGrid(game);
        printPowerUp(game);
        command = getch();
        cValue = (unsigned char)command;

        switch (cValue) {

        case KEY_MOVE_RIGHT:
          moveRight(game);
          break;
          
        case KEY_MOVE_LEFT:
          moveLeft(game);
          break;
        
        case KEY_ROT_RIGHT:
          rotateRight(game);
          break;
        
        case KEY_ROT_LEFT:
          rotateLeft(game);
          break;
        
        case KEY_DROP:
        {
          if(!drop(game)) {
            gameOver(game);
            return false;
          }          
          break;
        }
        
        case KEY_FINISH:
            gameEnded(game);
            return true;

        case KEY_GRAVITY:
        {
          if(game.gravity_mode_on){
            game.gravity_mode_on = false;
          }
          else{
            game.gravity_mode_on = true;
          }
          applyGravity(game);
          int blockRows = game.active_rotation->rows;
          int blockCols = game.active_rotation->cols;

        for(int i = 0; i < blockRows; i++) {
            for (int j = 0 ; j < blockCols; j++) {
                game.grid[i+game.active_rotation->tlRowOnGrid][j+game.active_rotation->tlColOnGrid]=game.active_rotation->shape[i][j];
            }
        }  
        }
            default:
                break;
      }

    }
    return true;
}
void GameController::printControls(){
    mvprintw(0,0,"[q] Rotate Left  [e] Rotate Right");
    mvprintw(1,0,"[a] Move Left    [d] Move Right");
    mvprintw(2,0,"[s] Drop         [g] Toggle Gravity");
    mvprintw(3,0,"[o] Exit Game");
}

bool GameController::checkPowerUp(BlockFall& game) {
    unsigned long blockRows = game.power_up.size();
    unsigned long blockCols = game.power_up[0].size();

    for(int i = 0; i <= game.rows - blockRows; ++i){
        for(int j = 0; j <= game.cols - blockCols; ++j) {
            bool found = true;
            for (int m = 0; m < blockRows && found; ++m) {
                for (int n = 0; n < blockCols && found; ++n) {
                    if((game.grid[i+m][j+n] != game.power_up[m][n])) {
                        found = false;
                    }

                }
            } 
            if(found) {
                int occupiedCellCount=countOccupiedCells(game.rows, game.cols, game.grid);
                game.current_score= game.current_score +1000 + occupiedCellCount;
                return true;
            }
        }
    }
    return false;
}

void GameController::cleanGrid(BlockFall& game){
    for(int i = 0; i < game.rows; i++){
        for(int j = 0; j < game.cols; j++) {
            game.grid[i][j] = false;
        }
    }
}

int GameController::countOccupiedCells(int rows, int cols, vector<vector<bool>> shape){
    int occupiedCells = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(shape[i][j] != 0){
                occupiedCells++;
            }
        }
    }
    return occupiedCells;
}
void GameController::writeBlockToGrid(BlockFall& game, unsigned long rowOfGrid) {
    //calculate score to increase
    int occupiedCells = countOccupiedCells(game.active_rotation->rows, game.active_rotation->cols, game.active_rotation->shape);

    game.current_score+=(rowOfGrid - game.active_rotation->tlRowOnGrid) * occupiedCells;

    //write block to the grid
    for(int i = 0; i < game.active_rotation->rows; i++) {
        for(int j = 0; j < game.active_rotation->cols; j++) {
            if(game.active_rotation->shape[i][j]==1) {
                game.grid[i+rowOfGrid][j+game.active_rotation->tlColOnGrid] = game.active_rotation->shape[i][j];
            }
        }
    }
}
int GameController::drop(BlockFall& game) {
    dropTheBlock(game);
    if(game.gravity_mode_on) {
        applyGravity(game);

    }
    if(checkPowerUp(game)){
        cleanGrid(game);
    }
    else{
        freeTheGrid(game);
    }
    game.active_rotation = game.active_rotation->next_block;
    return initiateBlock(game);
}
void GameController::printPowerUp(BlockFall& game){
    int h, w;
    getmaxyx(stdscr, h, w);
    mvprintw(h/2-5,w/2+15,"PowerUp shape:");
    for(int i =0; i < game.power_up.size(); i++){
        for(int j=0; j < game.power_up[0].size(); j++){
            if(game.power_up[i][j]==0) {
                mvprintw(i+h/2-4,w/2+15+j,unoccupiedCellChar);
            }
            else {
                mvprintw(i+h/2-4,w/2+15+j,occupiedCellChar);
            }
        }
    }
    move(0,0);
}
void GameController::gameEnded(BlockFall& game){
    clear();
    int h, w, jX;
    getmaxyx(stdscr, h, w);
    game.leaderboard.insert_new_entry(new LeaderboardEntry(game.current_score, std::time(nullptr), game.player_name));
    game.leaderboard.write_to_file(game.leaderboard_file_name);

    mvprintw(h/2-10,w/2-18,"EXIT BUTTON PRESSED. Game is ended.");
    mvprintw(h/2-6,w/2-15,"Final grid and score: %lu", game.current_score);
    
    for(int i = 0; i<game.rows; i++) {
        for(int j = 0; j < game.cols; j++) {
            if(game.grid[i][j]==0) {
                mvprintw(i+h/2-5,w/2-15+j,unoccupiedCellChar);
            }
            else {
                mvprintw(i+h/2-5,w/2-15+j,occupiedCellChar);
            }
            jX = j;
        }
        mvprintw(i+h/2-5,w/2-15+jX+1,"\n");
    }
    game.leaderboard.print_leaderboard();
}
void GameController::gameOver(BlockFall& game) {
    clear();
    int h, w,jX;
    getmaxyx(stdscr, h, w);
    game.leaderboard.insert_new_entry(new LeaderboardEntry(game.current_score, std::time(nullptr), game.player_name));
    game.leaderboard.write_to_file(game.leaderboard_file_name);

    mvprintw(h/2-5,w/2-45,"GAME OVER!");
    mvprintw(h/2-4,w/2-45,"Next block that couldn't fit:");
    for(int i =0; i < game.active_rotation->rows; i++){
        for(int j=0; j < game.active_rotation->cols; j++){
            if(game.active_rotation->shape[i][j]==0) {
                mvprintw(i+h/2-3,w/2-45+j,unoccupiedCellChar);
            }
            else {
                mvprintw(i+h/2-3,w/2-45+j,occupiedCellChar);
            }
            jX = j;   
        }
    }
    
    mvprintw(h/2-6,w/2-15,"Final grid and score: %lu", game.current_score);
    
    for(int i = 0; i<game.rows; i++) {
        for(int j = 0; j < game.cols; j++) {
            if(game.grid[i][j]==0) {
                mvprintw(i+h/2-5,w/2-15+j,unoccupiedCellChar);
            }
            else {
                mvprintw(i+h/2-5,w/2-15+j,occupiedCellChar);
            }
            jX = j;
        }
        mvprintw(i+h/2-5,w/2-15+jX+1,"\n");
    }
    game.leaderboard.print_leaderboard();
}
void GameController::dropTheBlock(BlockFall& game) {
        removeBlockFromGrid(game, game.active_rotation);

        unsigned long blockRows = game.active_rotation->shape.size();
        unsigned long blockCols = game.active_rotation->shape[0].size();

        for(int gridRow=0; gridRow < game.rows; gridRow++) {
            for(int blockRow = 0; blockRow < blockRows; blockRow++) {
                for(int blockCol = 0; blockCol < blockCols; blockCol++) {
                    if(gridRow+blockRow>game.rows-1) {
                        writeBlockToGrid(game, game.rows - blockRows);
                        return;
                    }
                    else if(game.grid[gridRow+blockRow][game.active_rotation->tlColOnGrid+blockCol]==1 && game.active_rotation->shape[blockRow][blockCol]==1) {
                        writeBlockToGrid(game, gridRow-1);
                        return;
                    }
                }
            }
        }
    writeBlockToGrid(game, game.rows - blockRows);
}

void GameController::freeTheGrid(BlockFall& game) {
    bool isFreeRow;
    do {
        isFreeRow = checkToFree(game);
    }
    while(isFreeRow);
}

bool GameController::checkToFree(BlockFall& game){
    bool findZero;

    for(int i = game.rows-1; i >= 0; i--){
        findZero = false;
        for(int j = 0; j < game.cols; j++) {
            if(game.grid[i][j]==0){
                findZero = true;
            }
        }

        if(!findZero) {
            freeRow(game, i);
            return true;

        }
    }
    return false;
}

void GameController::freeRow(BlockFall& game,int row){
    for(int i = row; i == row; i++){
        for(int j = 0; j < game.cols; j++) {
            game.grid[i][j] = false;
        }
    }   

    for(int i = row; i > 0; i--){
        for(int j = 0; j < game.cols; j++) {
            game.grid[i][j] = game.grid[i-1][j];
        }
    }    
    game.current_score += game.cols;
}



void GameController::applyGravity(BlockFall& game) {
    removeBlockFromGrid(game, game.active_rotation);

    for(int j = 0; j < game.cols; j++) {
        int occupiedCellCounter = 0;
        for(int i = 0; i<game.rows; i++) {
            if(game.grid[i][j]==1) {
                occupiedCellCounter++;
                game.grid[i][j]= false;
            }
        }
        for(int reverseI = game.rows-1; occupiedCellCounter>0; occupiedCellCounter--) {
            game.grid[reverseI][j] = true;
            reverseI--;
        }
    }
    if(checkPowerUp(game)) {
        cleanGrid(game);
    }
    else{
        freeTheGrid(game);
    }

}
int GameController::rotateRight(BlockFall& game){
    removeBlockFromGrid(game, game.active_rotation);
    game.active_rotation->right_rotation->tlRowOnGrid = game.active_rotation->tlRowOnGrid;
    game.active_rotation->right_rotation->tlColOnGrid = game.active_rotation->tlColOnGrid;

    Block* rightRotation = game.active_rotation->right_rotation;
    if(rightRotation->tlColOnGrid + rightRotation->cols  > game.cols){
        writeBlockToGrid(game,game.active_rotation->tlRowOnGrid);
        return -1;
    }

    for(int i = 0; i < rightRotation->rows; i++) {
        for(int j = 0; j < rightRotation->cols; j++) {
            if(rightRotation->shape[i][j] == 1 &&  game.grid[i+rightRotation->tlRowOnGrid][j+rightRotation->tlColOnGrid]!=0) {
                writeBlockToGrid(game,game.active_rotation->tlRowOnGrid);
                return -1;
            }
        }
    }

        for(int i = 0; i < rightRotation->rows; i++) {
            for (int j = 0 ; j < rightRotation->cols; j++) {
                game.grid[i+rightRotation->tlRowOnGrid][j+rightRotation->tlColOnGrid]=rightRotation->shape[i][j];
            }
        }
    game.active_rotation = rightRotation;
    return 0;
}
int GameController::rotateLeft(BlockFall& game){
    removeBlockFromGrid(game, game.active_rotation);
    game.active_rotation->left_rotation->tlRowOnGrid = game.active_rotation->tlRowOnGrid;
    game.active_rotation->left_rotation->tlColOnGrid = game.active_rotation->tlColOnGrid;

    Block* leftRotation = game.active_rotation->left_rotation;
    if(leftRotation->tlColOnGrid + leftRotation->cols  > game.cols){
        writeBlockToGrid(game,game.active_rotation->tlRowOnGrid);
        return -1;
    }

    for(int i = 0; i < leftRotation->rows; i++) {
        for(int j = 0; j < leftRotation->cols; j++) {
            if(leftRotation->shape[i][j] && game.grid[i+leftRotation->tlRowOnGrid][j+leftRotation->tlColOnGrid]!=0) {
                writeBlockToGrid(game,game.active_rotation->tlRowOnGrid);
                return -1;
            }
        }
    }
    game.active_rotation = game.active_rotation->left_rotation;

        for(int i = 0; i < game.active_rotation->rows; i++) {
            for (int j = 0 ; j < game.active_rotation->cols; j++) {
                int value = 0;
                if(game.active_rotation->shape[i][j]){
                    value = 1;
                }
                game.grid[i+game.active_rotation->tlRowOnGrid][j+game.active_rotation->tlColOnGrid]=value;
            }
        }
    return 0;
}

void GameController::printGrid(BlockFall& game){
    int h, w;
    getmaxyx(stdscr, h, w);
    mvprintw(h/2-8,w/2-15,"Score: %lu \n", game.current_score);
    mvprintw(h/2-7,w/2-15,"High Score: %lu \n", game.leaderboard.head_leaderboard_entry->score);
    mvprintw(h/2-6,w/2-15,"Gravity Mode: %s \n", game.gravity_mode_on ? "On" : "Off");
    int jX;
    for(int i = 0; i<game.rows; i++) {
        for(int j = 0; j < game.cols; j++) {
            if(game.grid[i][j]==0) {
                mvprintw(i+h/2-5,w/2-15+j,unoccupiedCellChar);
            }
            else {
                mvprintw(i+h/2-5,w/2-15+j,occupiedCellChar);
            }
            jX = j;
        }
        mvprintw(i+h/2-5,w/2-15+jX+1,"\n");
    }
}



int GameController::moveRight(BlockFall& game) {
    int blockRows = game.active_rotation->rows;
    int blockCols = game.active_rotation->cols;

    //this may encounter an error
    if(game.active_rotation->tlColOnGrid+blockCols>=game.cols){
        return -10;
    }

    for(int i = 0; i < blockRows; i++) {

            if(game.active_rotation->shape[i][blockCols-1]==1  && game.grid[i+game.active_rotation->tlRowOnGrid][game.active_rotation->tlColOnGrid+blockCols]!=0) {
                return -15;
            }  
    }


    removeBlockFromGrid(game, game.active_rotation);
    


    for(int i = 0; i < blockRows; i++) {
        for (int j = 0 ; j < blockCols; j++) {
            game.grid[i+game.active_rotation->tlRowOnGrid][j+game.active_rotation->tlColOnGrid+1]=game.active_rotation->shape[i][j];
        }
    }   
    game.active_rotation->tlColOnGrid += 1;
    
    return 0;
}


int GameController::moveLeft(BlockFall& game) {
    if(game.active_rotation->tlColOnGrid-1<0){
        return -10;
    }
    
    for(int i = 0; i < game.active_rotation->rows; i++) {
        if(game.active_rotation->shape[i][0]==1 && game.grid[i+game.active_rotation->tlRowOnGrid][ game.active_rotation->tlColOnGrid-1]!=0){
            return -15;
        }
    }

    removeBlockFromGrid(game, game.active_rotation);
    
    for(int i = 0; i < game.active_rotation->rows; i++) {
        for (int j = 0 ; j < game.active_rotation->cols; j++) {
            game.grid[i+game.active_rotation->tlRowOnGrid][j+game.active_rotation->tlColOnGrid-1]=game.active_rotation->shape[i][j];
        }
    }
    game.active_rotation->tlColOnGrid -= 1;
    return 0;
}

void GameController::removeBlockFromGrid(BlockFall& game, Block* block) { 
    if(block->cols+block->tlColOnGrid  > game.cols) {
        return;
    }

    for(int i = 0; i < block->rows; i++) {
        for (int j = 0 ; j < block->cols; j++) {
            game.grid[i+block->tlRowOnGrid][j+block->tlColOnGrid]=false;
        }
    }   
}

bool GameController::initiateBlock(BlockFall& game) {
    if(game.active_rotation==nullptr) {
        game.active_rotation = game.initial_block;
    }

    Block* block = game.active_rotation;

    for(int i = 0; i < block->rows; i++) {
        for (int j = 0 ; j < block->cols; j++) {
            if(game.grid[i][j]!=0){
                return false; 
            } 
        }
    }

    for(int i = 0; i < block->rows; i++) {
        for (int j = 0 ; j < block->cols; j++) {
            game.grid[i][j]=block->shape[i][j];
        }
    }   

    return true;
}



