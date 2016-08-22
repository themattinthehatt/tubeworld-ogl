//
// Created by mattw on 8/16/16.
//
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TubeTraveller.h"
#include "../core/loaders/loadShaders.h"

TubeTraveller::TubeTraveller(
    GLuint numCubesHorizontal_, GLuint numCubesVertical_, GLuint numCenters_,
    const bool *keysPressed_, const bool *keysToggled_)
        :
    cubearray(numCubesHorizontal_, numCubesVertical_, numCenters_),
    path(numCenters_, keysPressed_, keysToggled_) {

    path.spacing = cubearray.spacing;
}

void TubeTraveller::update(Camera &cam, Player &player) {
    path.update(cam, player);
    cubearray.update(path, cam);
}

void TubeTraveller::draw() {
    cubearray.draw();
}

void TubeTraveller::clean() {
    path.clean();
    cubearray.clean();
}

