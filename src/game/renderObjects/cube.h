#ifndef GAME_CUBE_H_
#define GAME_CUBE_H_
#include "game/renderObjects/vertex.h"

static const struct vertex cubeVertices[36] = {
        // position        UV         color
        // Front face
        {-0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 0.0}, // Top left front
        {0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right front
        {-0.5, -0.5, 0.5, 0.0, 1.0, 0.0, 1.0, 0.0}, // Bottom left front

        {0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right front
        {0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 1.0, 0.0}, // Bottom right front
        {-0.5, -0.5, 0.5, 0.0, 1.0, 0.0, 1.0, 0.0}, // Bottom left front

        // Back face
        {-0.5, 0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0}, // Top left back
        {-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0, 0.0}, // Bottom left back
        {0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right back

        {0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right back
        {-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0, 0.0}, // Bottom left back
        {0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 1.0, 0.0}, // Bottom right back

        // Right face
        {0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right front
        {0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0},// Top Back right
        {0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 1.0, 0.0}, // Bottom right front
        
        {0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top Back right
        {0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 1.0, 0.0},// Bottom right back
        {0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 1.0, 0.0}, // Bottom right front

        // Left face
        {-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0},// Top Back left 
        {-0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top left front
        {-0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 1.0, 0.0}, // Bottom left front

        {-0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 1.0, 0.0},// Bottom right back
        {-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top Back right
        {-0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 1.0, 0.0}, // Bottom right front


        // Top face
        {-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top left back (top)
        {0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right back (top)
        {-0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Bottom left top

        {0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right back (top)
        {0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right front                                          
        {-0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Bottom left top

        // Bottom face
        {0.5, -0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top right back (top)
        {-0.5, -0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top left back (top)
        {-0.5, -0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Bottom left top

        {0.5, -0.5, 0.5, 1.0, 0.0, 0.0 ,1.0 ,0.0}, // Bottom right front
        {0.5, -0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Top left back (top)
        {-0.5, -0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0}, // Bottom left top
};


#endif // GAME_CUBE_H_
