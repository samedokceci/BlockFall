#include "BlockFall.h"
#include <fstream>
#include <iostream>


BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    std::fstream inputFile;
    inputFile.open(input_file, std::ios::in);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << input_file << std::endl;
        return;
    }

    std::vector<bool> currentVector;
    std::string currentLine, line;

    Block* currentBlock = new Block();
    initial_block = currentBlock;
    active_rotation = currentBlock;

    int numLines = 0;
    int count = 0;

    while (std::getline(inputFile, line)) {
        if(line[0]=='[' || line[0]=='1' || line[0]=='0') {
        numLines++;
        }
    }

    inputFile.close();


    inputFile.open(input_file, std::ios::in);
    while (numLines>0) {
        currentVector.clear();
        inputFile >> currentLine;
        for (int i = 0; i < currentLine.length(); i++){
            if (currentLine[i] == '1' || currentLine[i]=='0') {
                currentVector.push_back(((currentLine[i]=='1') ? true : false));
            } 
        }
        currentBlock->shape.push_back(currentVector);
        currentVector.clear();

        if (currentLine[currentLine.length()-1] == ']') {
            count++;
            Block* temp = new Block();
    
            currentBlock->rows = currentBlock->shape.size();
            currentBlock->cols = currentBlock->shape[0].size();

            currentBlock->next_block = temp;
            createConnections(currentBlock);
            currentBlock = temp;
        }
        numLines--;
    }
    inputFile.close();

    
    Block* current = initial_block->next_block; 

    for (int i = 0; i < count;i++) {
        if(i == count - 3 ) {
            power_up = current->next_block->shape;
            delete current->next_block->next_block;
            deleteBlock(current->next_block);
            current->next_block = nullptr;
            break;
        }   
        current = current->next_block;
    }
}

void BlockFall::deleteBlock(Block* block){
    
    Block* firstRotation = block->right_rotation;
    Block* secondRotation = firstRotation->right_rotation;
    Block* thirdRotation = secondRotation->right_rotation;

    delete block;
    delete firstRotation;
    delete secondRotation;
    delete thirdRotation;
}

void BlockFall::initialize_grid(const string &input_file) {

    std::fstream inputFile;
    inputFile.open(input_file, std::ios::in);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << input_file << std::endl;
        return;
    }

    vector<bool> lineVector;
    std::string line;
    int col=0;
    int row=0;
    while (std::getline(inputFile, line)) {
        if(line[0]=='0' || line[0]=='1') {
            for (int i = 0; i < line.length(); i++){
                if (line[i] == '1' || line[i]=='0') {
                    lineVector.push_back(((line[i]=='1') ? true : false));
                    col++;
                } 
            }       
            row++;
        this->grid.push_back(lineVector);
        lineVector.clear();
        }
    }
    this->cols = col/row;
    this->rows = row;
}

void BlockFall::createConnections(Block* block) {
    block->rows = block->shape.size();
    block->cols = block->shape[0].size();
   
    Block* firstRotatedBlock = new Block();
    createRotations(block,firstRotatedBlock);
    block->right_rotation = firstRotatedBlock;
    firstRotatedBlock->left_rotation = block;
    firstRotatedBlock->next_block = block->next_block;

    Block* secondRotatedBlock = new Block();
    createRotations(firstRotatedBlock, secondRotatedBlock);
    firstRotatedBlock->right_rotation = secondRotatedBlock;
    secondRotatedBlock->left_rotation = firstRotatedBlock;
    secondRotatedBlock->next_block = block->next_block;


    Block* thirdRotatedBlock = new Block();
    createRotations(secondRotatedBlock, thirdRotatedBlock);
    secondRotatedBlock->right_rotation = thirdRotatedBlock;
    thirdRotatedBlock->left_rotation = secondRotatedBlock;
    thirdRotatedBlock->next_block = block->next_block;


    thirdRotatedBlock->right_rotation = block;
    block->left_rotation = thirdRotatedBlock;
}

void BlockFall::createRotations( Block* block, Block* rightRotatedBlock) {
        int numRows = block->shape.size();
        int numColumns = block->shape[0].size();
        std::vector<std::vector<bool>> shape(numColumns, std::vector<bool>(numRows, 0));
        rightRotatedBlock->shape = shape;

        for(int i = 0; i < numRows; i++) {
            for (int j = 0; j < numColumns; j++) {
                int newI = j;
                int newJ = numRows - 1 - i;

                rightRotatedBlock->shape[newI][newJ] = block->shape[i][j];
                
            }
        }
        rightRotatedBlock->rows = rightRotatedBlock->shape.size();
        rightRotatedBlock->cols = rightRotatedBlock->shape[0].size();

}

BlockFall::~BlockFall() {
    Block* currentBlock = initial_block;
    while(currentBlock!=nullptr) {
        Block* tempNextBlock = currentBlock->next_block;
        
        deleteBlock(currentBlock);

        currentBlock = tempNextBlock;
    }
    active_rotation = nullptr;
    initial_block = nullptr;
}
