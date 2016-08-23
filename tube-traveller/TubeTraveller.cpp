//
// Created by mattw on 8/16/16.
//

#include "TubeTraveller.h"

TubeTraveller::TubeTraveller(
    GLuint numCubesHorizontal_, GLuint numCubesVertical_, GLuint numCenters_,
    const bool *keysPressed_, const bool *keysToggled_)
        :
    cubearray(numCubesHorizontal_, numCubesVertical_, numCenters_,
              keysPressed_, keysToggled_),
    path(numCenters_, keysPressed_, keysToggled_) {

    path.spacing = cubearray.spacing;
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

