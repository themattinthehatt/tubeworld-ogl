//
// Created by mattw on 8/21/16.
//

#ifndef TUBEWORLD_PATHUSERINPUT_H
#define TUBEWORLD_PATHUSERINPUT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Camera.h"

class PathUserInput {
private:
public:

    enum PlayerMode {
        PLAYER_FREE,
        PLAYER_BOUND,
        MAX_PLAYER_MODES
    };

    GLuint numCenters;
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
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;

    // constructor
    PathUserInput(GLuint numCenters,
                  const bool *keysPressed,
                  const bool *keysToggled);
    // update dynamics of tube positions
    void update(Camera &cam, Player &player);
    // clean up dynamically allocated memory
    void clean();
};


#endif //TUBEWORLD_PATHUSERINPUT_H
