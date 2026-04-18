#include "game/gameClasses/chunkManager/chunkManager.h"
#include "game/gameClasses/camera/camera.h"
#include "game/renderObjects/chunk.h"
#include "rtELog/rtELog.h"
#include "rtEMath/rtEMath.h"
#include "rtERenderer/rtER_VK_struct_definitions.h"
#include "game/gameClasses/camera/cameraStructDefinition.h"
#include "rtERenderer/rtERenderer.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void worldCoordinatesToChunkCoordinates(const vec3 coords, vec3 dest) {
        vec3 chunkCoords ={
                coords[0] / CHUNK_X_SIZE,
                0, // height is irrelevant
                coords[1] / CHUNK_Z_SIZE
        };

        memcpy(dest, chunkCoords, sizeof(float) * 3);
}

struct rtGame_chunkManager {
        uint32_t renderDistance;

        uint32_t loadedChunksCount;
        struct chunk* loadedChunks;

        const struct rtGame_camera* pPlayerCamera;

        struct rtERenderer* pRenderer;
        rter_vbo_t chunkVBO;
};

struct rtGame_chunkManager* rtGame_createChunkManager(
                const uint32_t renderDistance, 
                const struct rtGame_camera* pPlayerCamera,
                struct rtERenderer* pRenderer
                ) {
        
        struct rtGame_chunkManager* newManager = malloc(sizeof(struct rtGame_chunkManager));

        newManager->renderDistance = renderDistance;
        newManager->loadedChunksCount = 0;
        newManager->loadedChunks = nullptr;
        newManager->pPlayerCamera = pPlayerCamera;
        newManager->pRenderer = pRenderer;
        newManager->chunkVBO = nullptr;
        rtER_createVertexBuffer(newManager->pRenderer, &newManager->chunkVBO);

        return newManager;
}

void rtGame_chunkManagerUpdate(struct rtGame_chunkManager* chunkManager) {
        // Not yet ready to be ran more than once
        vec3 playerChunkCoordinates = {};
        worldCoordinatesToChunkCoordinates(chunkManager->pPlayerCamera->cameraPos, playerChunkCoordinates);

        // Loop through all possible chunk locations and load a chunk there if not already loaded
        for (int x = -chunkManager->renderDistance; x < (int)chunkManager->renderDistance; x++) {
                for (int z = -chunkManager->renderDistance; z < (int)chunkManager->renderDistance; z++) {
                        vec3 potentialPosition = {x, 0, z};
                        float distance = rtEMath_vec3GetMagnitude(potentialPosition);

                        if (distance <= chunkManager->renderDistance) {
                               // load chunk 
                                chunkManager->loadedChunksCount++;
                                chunkManager->loadedChunks = realloc(chunkManager->loadedChunks, sizeof(struct chunk) * chunkManager->loadedChunksCount);
                                (chunkManager->loadedChunks[chunkManager->loadedChunksCount-1]) = rtGame_createChunk(potentialPosition);

                                // Initialize chunk mesh data
                                rtGame_chunkCreateMesh(&chunkManager->loadedChunks[chunkManager->loadedChunksCount-1]);
                        } else {
                                continue;
                        }
                }
        }


        // Get all chunk data into a contiguous buffer, then send it to renderer via vertex buff
        unsigned char* vboData = nullptr;
        size_t offset = 0;
        for (size_t i = 0; i < chunkManager->loadedChunksCount; i++) {
                size_t size = sizeof(struct vertex) * 36 * rtGame_chunkGetMeshSize(&chunkManager->loadedChunks[i]);
                vboData = realloc(
                                vboData, 
                                 size + offset 
                                 );
                memcpy(vboData + offset, rtGame_chunkGetMeshPtr(&chunkManager->loadedChunks[i]), size);
                offset+=size;
                rtELog_debug_logInfo("%d", offset);
        }

        rtER_bufferVertexData(chunkManager->pRenderer, chunkManager->chunkVBO, (void*)vboData, offset);
        rtER_bindVertexBuffer(chunkManager->pRenderer, chunkManager->chunkVBO);

}

void rtGame_cleanupChunkManager(
                struct rtGame_chunkManager** chunkManager
                ) {
        free(*chunkManager);
}

