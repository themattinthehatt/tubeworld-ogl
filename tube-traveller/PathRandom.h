//
// Created by mattw on 8/24/16.
//

#ifndef TUBEWORLD_PATHRANDOM_H
#define TUBEWORLD_PATHRANDOM_H

#include <vector>
#include <random>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Player.h"
#include "PathGenerator.h"

class PathRandom : public PathGenerator {
private:
public:

    // for random walk generation
    std::mt19937::result_type seed;
    GLfloat deltaHorzAngleUpdate;
    GLfloat deltaVertAngleUpdate;
    GLfloat maxDeltaHorzAngle;
    GLfloat maxDeltaVertAngle;

    // for moving camera
    GLfloat distanceTotal;
    GLfloat distanceRemaining;
    GLfloat distanceToTravel;
    GLfloat propOfPathTraveled;
    GLint segsToAdd;

    // indices for manipulating tube info vectors above
    GLint currElement;
    GLint nextElement;
    GLint firstElement;
    GLint lastElement;

    // movement variables that can be manipulated by keyboard
    GLfloat speed = 10.f;
    GLfloat maxSpeed = 500.f;
    GLfloat minSpeed = 0.0f;
    GLfloat horzAngleUpdateRange;
    GLfloat minHorzAngleUpdateRange;
    GLfloat maxHorzAngleUpdateRange;
    GLfloat vertAngleUpdateRange;
    GLfloat minVertAngleUpdateRange;
    GLfloat maxVertAngleUpdateRange;

    IOHandler &io;

    // constructor
    PathRandom(GLint numCenters);
    // update dynamics of tube positions
    void update(Player &player);
    // clean up dynamically allocated memory
    void clean();

};


#endif //TUBEWORLD_PATHRANDOM_H