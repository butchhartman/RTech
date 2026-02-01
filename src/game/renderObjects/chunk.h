#ifndef GAME_CHUNK_H_
#define GAME_CHUNK_H_
#include "game/blockIDs.h"

#define CHUNK_X_SIZE 8
#define CHUNK_Y_SIZE 16
#define CHUNK_Z_SIZE 8

struct chunk {

        enum BLOCK_IDS blocks[CHUNK_X_SIZE][CHUNK_Y_SIZE][CHUNK_Z_SIZE];
        
};

#endif // CHUNK_H_
