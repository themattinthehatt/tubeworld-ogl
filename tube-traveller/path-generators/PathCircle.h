//
// Created by mattw on 8/27/16.
//

#ifndef TUBEWORLD_PATHCIRCLE_H
#define TUBEWORLD_PATHCIRCLE_H


#include "PathGenerator.h"

class PathCircle : public PathGenerator {
private:
public:

    GLfloat radius;

    IOHandler &io;

    // constructor
    PathCircle(GLint numCenters, GLfloat spacinig);
    // update dynamics of tube positions
    void update(Player &player);
    // clean up dynamically allocated memory
    void clean();

};


#endif //TUBEWORLD_PATHCIRCLE_H
