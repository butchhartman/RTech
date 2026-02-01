#ifndef GAME_CHUNK_H_
#define GAME_CHUNK_H_

#define CHUNK_X_SIZE 2
#define CHUNK_Y_SIZE 4
#define CHUNK_Z_SIZE 2

struct chunk;

struct chunk* rtGame_createChunk();

void rtGame_chunkCreateMesh(struct chunk* chunk);

struct vertex* rtGame_chunkGetMeshPtr(struct chunk* chunk);

size_t rtGame_chunkGetMeshSize(struct chunk* chunk);

void rtGame_destroyChunk(struct chunk** chunk);


#endif // CHUNK_H_
