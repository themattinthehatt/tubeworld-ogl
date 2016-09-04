//
// Created by mattw on 8/16/16.
//

#include "TubeTraveller.h"
#include "PathCircle.h"
#include "PathUserInput.h"
#include "PathRandom.h"

TubeTraveller::TubeTraveller(GLint numCenters) : io(IOHandler::getInstance()){

    enum PathGeneratorType {
        PATH_CIRCLE,
        PATH_USER,
        PATH_RANDOM,
        MAX_NUM_PATHS
    };
    enum TubeType {
        TUBE_CUBES_SQ,
        TUBE_CUBES_CIRC,
        MAX_NUM_TUBES
    };

    PathGeneratorType pathType = PATH_CIRCLE;
    TubeType tubeType = TUBE_CUBES_CIRC;

    // select path type
    switch (pathType) {
        case PATH_CIRCLE:
            path = new PathCircle(numCenters);
            break;
        case PATH_USER:
            path = new PathUserInput(numCenters);
            break;
        case PATH_RANDOM:
            path = new PathRandom(numCenters);
            break;
        default:
            path = new PathRandom(numCenters);
    }

    // select tube type
    switch (tubeType) {
        case TUBE_CUBES_SQ:
            tube = new CubeTube(numCenters, CubeTube::SQUARE_OF_SQUARES);
            break;
        case TUBE_CUBES_CIRC:
            tube = new CubeTube(numCenters, CubeTube::CIRCLE_OF_SQUARES);
            break;
        default:
            tube = new CubeTube(numCenters, CubeTube::CIRCLE_OF_SQUARES);
    }
}

void TubeTraveller::update(Camera &cam, Player &player) {

    // toggle camera mode
    if (io.keysToggled[GLFW_KEY_SPACE] != playerModeTrigger) {
        playerModeTrigger = !playerModeTrigger;
        playerMode = (playerMode + 1) % MAX_PLAYER_MODES;
    }
    switch (playerMode) {
        case PLAYER_BOUND:
        {
            // path class updates player
            path->update(player);
            break;
        }
        case PLAYER_FREE:
            // user input updates player
            player.update();
            break;
        default:
            player.update();
    }

    // compute view and projection matrices from player info
    cam.update(player);

    // update tube
    tube->update(path, cam);
}

void TubeTraveller::draw() {
    tube->draw();
}

void TubeTraveller::clean() {
    path->clean();
    tube->clean();
    delete path;
    delete tube;
}

