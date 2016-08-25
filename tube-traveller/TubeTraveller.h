//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

#ifndef TUBEWORLD_TUBETRAVELLER_H
#define TUBEWORLD_TUBETRAVELLER_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Camera.h"
#include "CubeTube.h"
#include "PathRandom.h"

class TubeTraveller {
private:


public:

    PathRandom path;
    CubeTube cubearray;

    // constructor
    TubeTraveller(GLint numCenters,
                  const bool *keysPressed_, const bool *keysToggled_);
    // update dynamics of cube array
    void update(Camera &cam, Player &player);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_CUBEARRAYRING_H
