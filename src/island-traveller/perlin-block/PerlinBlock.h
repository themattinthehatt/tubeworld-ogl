//
// Created by mattw on 1/8/17.
//

#ifndef TUBEWORLD_PERLINBLOCK_H
#define TUBEWORLD_PERLINBLOCK_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Island.h"
#include "../../core/Skybox.h"
#include "PerlinBlockPortal.h"
#include "PerlinBlockIsland.h"
#include "PerlinBlockLight.h"
#include "../../core/FramebufferObject.h"


class PerlinBlock : public Island {

private:


public:

    // Island.h contains parameters for fading

    // island components
    std::vector<PerlinBlockPortal> sketchPortals;
    std::vector<PerlinBlockPortal> tubePortals;

    PerlinBlockIsland *island;
    Skybox *skybox;
    GLint numSketchPortals;
    GLint numTubePortals;
    PerlinBlockLight *light;

    // for post-processing
    FramebufferObject *fbo;
    Shader *postShader;

    IOHandler &io;

    // constructor
    PerlinBlock();
    // update dynamics of island
    GLint update(Camera &cam, Player &player);
    // draw island to screen
    void draw();
    // draw island to framebuffer
    void renderOffscreen();
    // play with post-processing effects
    void postProcess();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_PERLINBLOCK_H
