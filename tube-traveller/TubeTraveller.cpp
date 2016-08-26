//
// Created by mattw on 8/16/16.
//

#include "TubeTraveller.h"

TubeTraveller::TubeTraveller(GLint numCenters) {

    enum PathGeneratorType {
        PATH_RANDOM,
        PATH_USER,
        MAX_NUM_PATHS
    };
    enum TubeType {
        TUBE_CUBES_SQ,
        TUBE_CUBES_CIRC,
        MAX_NUM_TUBES
    };

    PathGeneratorType pathType = PATH_RANDOM;
    TubeType tubeType = TUBE_CUBES_CIRC;

    // select path type
    switch (pathType) {
        case PATH_RANDOM:
            path = new PathRandom(numCenters);
            break;
        case PATH_USER:
            path = new PathUserInput(numCenters);
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
    path->update(player);
    tube->update(path, player, cam);
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

