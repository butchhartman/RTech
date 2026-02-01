#ifndef GAME_VERTEX_H_
#define GAME_VERTEX_H_

// Vertices must be defined clockwise, lest they be culled

struct vertex {
        float x, y, z;
        float u, v;
        float r, g, b;
};

#endif // GAME_VERTEX_H_
