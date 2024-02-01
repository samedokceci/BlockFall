#ifndef PA2_BLOCK_H
#define PA2_BLOCK_H

#include <vector>
#include <iostream>
using namespace std;

class Block {
public:

    vector<vector<bool>> shape; // Two-dimensional vector corresponding to the block's shape
    Block * right_rotation = nullptr; // Pointer to the block's clockwise neighbor block (its right rotation)
    Block * left_rotation = nullptr; // Pointer to the block's counter-clockwise neighbor block (its left rotation)
    Block * next_block = nullptr; // Pointer to the next block to appear in the game
    int tlRowOnGrid = 0;
    int tlColOnGrid = 0;
    int rows = 0;
    int cols = 0;

    bool operator==(const Block& other) const {
        return shape == other.shape;
    }

    bool operator!=(const Block& other) const {
        return shape != other.shape;
    }
};


#endif //PA2_BLOCK_H
