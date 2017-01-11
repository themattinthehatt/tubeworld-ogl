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

    // for fading into and out of island
    GLfloat fadeStep;
    GLfloat fadeTotal;
    glm::vec3 fadeColor;
    GLint fadeStepID;
    GLint fadeTotalID;
    GLint fadeColorID;


    // update island
    virtual GLint update(Camera &cam, Player &player) = 0;
    // draw island
    virtual void draw() = 0;
    // draw island to framebuffer
    virtual void renderOffscreen() = 0;
    // play with post-processing effects
    virtual void postProcess() = 0;
    // clean up VAOs, VBOs, etc.
    virtual void clean() = 0;

    // setters
    void setFadeStep(GLfloat fadeStep_) {fadeStep = fadeStep_; };
    void setFadeTotal(GLfloat fadeTotal_) {fadeTotal = fadeTotal_; };


};

#endif //TUBEWORLD_ISLAND_H
