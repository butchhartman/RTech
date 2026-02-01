#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "game/renderObjects/cube.h"
#include "game/renderObjects/vertex.h"
#include "game/renderObjects/chunk.h"
#include "game/blockIDs.h"
#include "rtELog/rtELog.h"

struct chunk {

        enum BLOCK_IDS blocks[CHUNK_X_SIZE][CHUNK_Y_SIZE][CHUNK_Z_SIZE];

        size_t meshSize;
        struct vertex* mesh;        

};

struct chunk* rtGame_createChunk() {
        struct chunk* chunk = malloc(sizeof(struct chunk));

        // A default chunk is a solid mass of cubes
        memset(chunk->blocks, BLOCK_STONE, sizeof(enum BLOCK_IDS) * CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE);

        chunk->meshSize = 0;
        chunk->mesh = nullptr;

        return chunk;
}

void rtGame_chunkCreateMesh(struct chunk* chunk) {
        uint32_t renderedCubeCount = 0;

        for (size_t x = 0; x < CHUNK_X_SIZE; x++) {
                for (size_t y = 0; y < CHUNK_Y_SIZE; y++) {
                        for (size_t z = 0; z < CHUNK_Z_SIZE; z++) {
                                if (chunk->blocks[x][y][z] != BLOCK_AIR) {

                                        struct vertex tempVerts[36];
                                        memcpy(tempVerts, cubeVertices, sizeof(struct vertex) * 36);


                                        for(size_t i = 0; i < 36; i++) {
                                                tempVerts[i].x += x;
                                                tempVerts[i].y += y;
                                                tempVerts[i].z += z;
                                        }


                                        if (chunk->meshSize < renderedCubeCount + 1 ) {
                                                // Very slow, at least at first. 
                                                chunk->mesh = realloc(chunk->mesh, sizeof(struct vertex) * (renderedCubeCount + 1)* 36);
                                        }

                                        memcpy(chunk->mesh + (36 * renderedCubeCount), tempVerts, sizeof(struct vertex) * 36);

                                        renderedCubeCount++;
                                }
                        }
                }
        }

        chunk->meshSize = renderedCubeCount;
}

struct vertex* rtGame_chunkGetMeshPtr(struct chunk* chunk) {
        return chunk->mesh;
}

size_t rtGame_chunkGetMeshSize(struct chunk* chunk) {
        return chunk->meshSize;
}

void rtGame_destroyChunk(struct chunk* chunk) {
        free(chunk->mesh);
        free(chunk);
        chunk = nullptr;
}

