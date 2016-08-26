//
// Created by mattw on 8/21/16.
//

#ifndef TUBEWORLD_PATHUSERINPUT_H
#define TUBEWORLD_PATHUSERINPUT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Player.h"
#include "PathGenerator.h"

class PathUserInput : public PathGenerator {
private:
public:

    GLfloat speed = 10.f;
    GLfloat deltaHorzAngle = 0.0f;
    GLfloat deltaVertAngle = 0.0f;
    GLfloat decayRightHorzAngle = 0.0f;
    GLfloat decayLeftHorzAngle = 0.0f;
    GLfloat decayUpVertAngle = 0.0f;
    GLfloat decayDownVertAngle = 0.0f;
    GLfloat growthRightHorzAngle = 0.0f;
    GLfloat growthLeftHorzAngle = 0.0f;
    GLfloat growthUpVertAngle = 0.0f;
    GLfloat growthDownVertAngle = 0.0f;

    IOHandler &io;

    // constructor
    PathUserInput(GLint numCenters);
    // update dynamics of tube positions
    void update(Player &player);
    // clean up dynamically allocated memory
    void clean();
};


#endif //TUBEWORLD_PATHUSERINPUT_H
