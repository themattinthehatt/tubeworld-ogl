//
// Created by mattw on 1/7/17.
//

#ifndef TUBEWORLD_ISLANDTRAVELLER_H
#define TUBEWORLD_ISLANDTRAVELLER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Skybox.h"
#include "Island.h"
#include "../test-sketches/Sketch01.h"

class IslandTraveller {
private:


public:

    // set up island info
    GLint islandEndCount;
    GLint islandCounter;
    GLint stopIslandFlag;

    GLint sketchEndCount;
    GLint sketchCounter;
    GLint stopSketchFlag;

    Island *island;
    Sketch01 *sketch;
    IOHandler &io;

    // constructor
    IslandTraveller();
    // update dynamics of island (output determines island/tube transition)
    bool update(Camera &cam, Player &player);
    // draw island
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_ISLANDTRAVELLER_H
