//
// Created by mattw on 8/26/16.
//
// virtual base class that requires implementations in derived classes (= 0)

#ifndef TUBEWORLD_PATHGENERATOR_H
#define TUBEWORLD_PATHGENERATOR_H


#include <GL/glew.h>
#include <vector>
#include "../../core/Player.h"

class PathGenerator {
public:

    // info for tube slices
    GLint numCenters;
    GLfloat spacing;

    // ugly hack to get some of the procedural texture
    // generation working
    GLint segsToAdd;
    GLint firstElement;
    GLint lastElement;

    // tube info
    std::vector<GLfloat> horizontalAngles;
    std::vector<GLfloat> verticalAngles;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> headings;
    std::vector<glm::vec3> ups;
    std::vector<glm::vec3> rights;

    // update dynamics of tube positions
    virtual void update(Player &player) = 0;
    // clean up dynamically allocated memory
    virtual void clean() = 0;

};


#endif //TUBEWORLD_PATHGENERATOR_H
