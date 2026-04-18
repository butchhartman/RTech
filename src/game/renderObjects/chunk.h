#ifndef GAME_CHUNK_H_
#define GAME_CHUNK_H_

#include "game/blockIDs.h"
#include "rtEMath/rtEMath.h"

#define CHUNK_X_SIZE 4
#define CHUNK_Y_SIZE 6
#define CHUNK_Z_SIZE 4

struct chunk {
        // This position is in chunk coordinates instead of block coordinates
        vec3 chunkPosition;

        enum BLOCK_IDS blocks[CHUNK_X_SIZE][CHUNK_Y_SIZE][CHUNK_Z_SIZE];

        size_t meshSize;
        struct vertex* mesh;        
};

struct chunk rtGame_createChunk(vec3 chunkPosition);

void rtGame_chunkCreateMesh(struct chunk* chunk);

struct vertex* rtGame_chunkGetMeshPtr(struct chunk* chunk);

size_t rtGame_chunkGetMeshSize(struct chunk* chunk);

#endif // CHUNK_H_
