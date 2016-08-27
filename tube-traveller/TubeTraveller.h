//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

#ifndef TUBEWORLD_TUBETRAVELLER_H
#define TUBEWORLD_TUBETRAVELLER_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "CubeTube.h"
#include "PathGenerator.h"

class TubeTraveller {
private:


public:

    PathGenerator *path;
    CubeTube *tube;

    IOHandler &io;

    enum PlayerMode {
        PLAYER_BOUND,
        PLAYER_FREE,
        MAX_PLAYER_MODES
    };
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;


    // constructor
    TubeTraveller(GLint numCenters);
    // update dynamics of cube array
    void update(Camera &cam, Player &player);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_CUBEARRAYRING_H
