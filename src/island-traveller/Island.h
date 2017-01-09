//
// Created by mattw on 1/7/17.
//

#ifndef TUBEWORLD_ISLAND_H
#define TUBEWORLD_ISLAND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Skybox.h"
#include "../core/Shader.h"

class Island {
private:
public:

    // update island
    virtual GLint update(Camera &cam, Player &player) = 0;
    // draw island
    virtual void draw() = 0;
    // clean up VAOs, VBOs, etc.
    virtual void clean() = 0;

};

#endif //TUBEWORLD_ISLAND_H
