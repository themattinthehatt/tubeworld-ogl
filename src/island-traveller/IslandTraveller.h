//
// Created by mattw on 1/7/17.
//

#ifndef TUBEWORLD_ISLANDTRAVELLER_H
#define TUBEWORLD_ISLANDTRAVELLER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Skybox.h"
#include "Island.h"

class IslandTraveller {
private:


public:

    Skybox *skybox;
    Island *island;

    IOHandler &io;

    // constructor
    IslandTraveller();
    // update dynamics of island
    bool update(Camera &cam, Player &player);
    // draw island
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_ISLANDTRAVELLER_H
