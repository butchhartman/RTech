#ifndef RTGAME_CHUNKMANAGER_H_
#define RTGAME_CHUNKMANAGER_H_

#include "game/gameClasses/camera/camera.h"
#include "rtERenderer/rtER_VK_struct_definitions.h"

struct rtGame_chunkManager;

struct rtGame_chunkManager* rtGame_createChunkManager(
                const uint32_t renderDistance, 
                const struct rtGame_camera* pPlayerCamera,
                struct rtERenderer* pRenderer
                );

void rtGame_chunkManagerUpdate(struct rtGame_chunkManager* chunkManager);

void rtGame_cleanupChunkManager(
                struct rtGame_chunkManager** chunkManager
                );
#endif // RTGAME_CHUNKMANAGER_H_
