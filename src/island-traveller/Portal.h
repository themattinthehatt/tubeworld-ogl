//
// Created by mattw on 1/14/17.
//

#ifndef TUBEWORLD_PORTAL_H
#define TUBEWORLD_PORTAL_H

#include "../core/Camera.h"
#include "../core/Player.h"

class Portal {

private:
public:

    // portal properties I
    glm::vec3 center;
    glm::vec3 heading;

    // update portal
    virtual void update(Camera cam, Player player) = 0;
    // draw portal
    virtual void draw() = 0;
    // clean portal
    virtual void clean() = 0;

    // getters
    glm::vec3 getCenter() { return center; };
    glm::vec3 getHeading() { return heading; };

};
#endif //TUBEWORLD_PORTAL_H
