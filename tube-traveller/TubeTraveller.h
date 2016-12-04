//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

// TODO
// TextureGenerator class
//      single image repeated init example
//      ROYGBIV init example for variable number of textures
//      1/f noise?
// figure out how to incorporate lighting into the TubeGenerator class (perhaps
//      leave out of TubeSimpleShape for simplicity - can always go back later)

#ifndef TUBEWORLD_TUBETRAVELLER_H
#define TUBEWORLD_TUBETRAVELLER_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "path-generators/PathGenerator.h"
#include "tube-generators/TubeGenerator.h"

class TubeTraveller {
private:


public:

    PathGenerator *path;
    TubeGenerator *tube;

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


#endif //TUBEWORLD_TUBETRAVELLER_H
