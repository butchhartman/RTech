#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "game/renderObjects/cube.h"
#include "game/renderObjects/vertex.h"
#include "game/renderObjects/chunk.h"
#include "game/blockIDs.h"
#include "rtELog/rtELog.h"
#include "rtEMath/rtEMath.h"


struct chunk rtGame_createChunk(vec3 chunkPosition) {
        struct chunk chunk;

        memcpy(chunk.chunkPosition, chunkPosition, RTEMATH_VEC3_SIZE);


        // A default chunk is a solid mass of cubes
        memset(chunk.blocks, BLOCK_STONE, sizeof(enum BLOCK_IDS) * CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE);

        chunk.meshSize = 0;
        chunk.mesh = nullptr;

        return chunk;
}

void rtGame_chunkCreateMesh(struct chunk* chunk) {
        uint32_t renderedCubeCount = 0;

        for (size_t x = 0; x < CHUNK_X_SIZE; x++) {
                for (size_t y = 0; y < CHUNK_Y_SIZE; y++) {
                        for (size_t z = 0; z < CHUNK_Z_SIZE; z++) {
                                if (chunk->blocks[x][y][z] == BLOCK_AIR) {
                                        continue;
                                }

                                struct vertex tempVerts[CUBE_VERTEX_COUNT];
                                memcpy(tempVerts, cubeVertices, CUBE_VERTEX_BYTES);

                                for(size_t i = 0; i < CUBE_VERTEX_COUNT; i++) {
                                        tempVerts[i].x += x + (chunk->chunkPosition[0] * CHUNK_X_SIZE);
                                        tempVerts[i].y += y; // Y shouldn't matter and should always be zero
                                        tempVerts[i].z += z + (chunk->chunkPosition[2] * CHUNK_Z_SIZE);
                                }


                                if (chunk->meshSize < renderedCubeCount + 1 ) {
                                        // Very slow, at least at first. 
                                        chunk->mesh = realloc(chunk->mesh, (renderedCubeCount + 1) * CUBE_VERTEX_BYTES);
                                }

                                memcpy(chunk->mesh + (CUBE_VERTEX_COUNT * renderedCubeCount), tempVerts, CUBE_VERTEX_BYTES);

                                renderedCubeCount++;
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
