//
// Created by mattw on 8/21/16.
//

#ifndef TUBEWORLD_PATHUSERINPUT_H
#define TUBEWORLD_PATHUSERINPUT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Player.h"

class PathUserInput {
private:
public:

    GLint numCenters;
    GLfloat spacing;

    GLfloat *horizontalAngles;
    GLfloat *verticalAngles;
    glm::vec3 *positions;
    glm::vec3 *headings;
    glm::vec3 *ups;
    glm::vec3 *rights;
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

    const bool *keysPressed;    // pointer to keysPressed array
    const bool *keysToggled;    // pointer to keysToggled array

    // constructor
    PathUserInput(GLint numCenters,
                  const bool *keysPressed,
                  const bool *keysToggled);
    // update dynamics of tube positions
    void update(Player &player);
    // clean up dynamically allocated memory
    void clean();
};


#endif //TUBEWORLD_PATHUSERINPUT_H
