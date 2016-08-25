//
// Created by mattw on 8/16/16.
//

#include "TubeTraveller.h"

TubeTraveller::TubeTraveller(GLint numCenters,
                             const bool *keysPressed_,
                             const bool *keysToggled_)
        :
    cubearray(numCenters, keysPressed_, keysToggled_),
    path(numCenters, keysPressed_, keysToggled_) {

    path.numCenters = cubearray.numCenters;
}

void TubeTraveller::update(Camera &cam, Player &player) {
    path.update(player);
    cubearray.update(path, player, cam);
}

void TubeTraveller::draw() {
    cubearray.draw();
}

void TubeTraveller::clean() {
    path.clean();
    cubearray.clean();
}

