//
// Created by mattw on 12/3/16.
//

#ifndef TUBEWORLD_TUBEGENERATOR_H
#define TUBEWORLD_TUBEGENERATOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../core/Global.h"
#include "../../core/Camera.h"
#include "../path-generators/PathGenerator.h"

class TubeGenerator {
private:
public:

    // update positions/angles of tube elements
    virtual void update(const PathGenerator *path, Camera &cam) = 0;
    // draw tube elements
    virtual void draw() = 0;
    // clean up VAOs, VBOs, etc.
    virtual void clean() = 0;

};

#endif //TUBEWORLD_TUBEGENERATOR_H
