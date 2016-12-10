//
// Created by mattw on 8/16/16.
//

#include <iostream>
#include "TubeTraveller.h"
#include "path-generators/PathCircle.h"
#include "path-generators/PathUserInput.h"
#include "path-generators/PathRandom.h"
#include "tube-generators/SimpleShapes.h"
#include "tube-generators/TextureCylinder.h"

TubeTraveller::TubeTraveller(GLint numCenters) : io(IOHandler::getInstance()){

// Copy of what is defined in header file
//    enum PathGeneratorType {
//        PATH_CIRCLE,
//        PATH_USER,
//        PATH_RANDOM,
//        MAX_NUM_PATHS
//    };
//    enum TubeType {
//        TUBE_CUBES_SQ,
//        TUBE_CUBES_CIRC,
//        TUBE_CYLINDER,
//        TUBE_TEXTURE_CYLINDER,
//        MAX_NUM_TUBES
//    };
//    enum TextureType {
//        TEXTURE_FILES_STATIC,
//        TEXTURE_RAINBOW,
//        TEXTURE_BINARY,
//        TEXTURE_NOISE,
//        MAX_NUM_TEXTURES
//    };

    PathGeneratorType pathType = PATH_RANDOM;
    TubeType tubeType = TUBE_TEXTURE_CYLINDER;

    // Notes
    // TextureType only applies to a TubeType of TUBE_TEXTURE_CYLINDER
    // TEXTURE_BINARY and TEXTURE NOISE only work with PATH_RANDOM
//    TextureType textureType = TEXTURE_RAINBOW;
    TextureType textureType = TEXTURE_BINARY;

    GLfloat centerSpacing = 2.f;

    // select path type
    switch (pathType) {
        case PATH_CIRCLE:
            path = new PathCircle(numCenters, centerSpacing);
            break;
        case PATH_USER:
            path = new PathUserInput(numCenters, centerSpacing);
            break;
        case PATH_RANDOM:
            path = new PathRandom(numCenters, centerSpacing);
            break;
        default:
            path = new PathRandom(numCenters, centerSpacing);
    }

    // select tube type
    switch (tubeType) {
        case TUBE_CUBES_SQ:
            tube = new SimpleShapes(numCenters, SimpleShapes::SQUARE_OF_SQUARES);
            break;
        case TUBE_CUBES_CIRC:
            tube = new SimpleShapes(numCenters, SimpleShapes::CIRCLE_OF_SQUARES);
            break;
        case TUBE_CYLINDER:
            tube = new SimpleShapes(numCenters, SimpleShapes::CYLINDER);
            break;
        case TUBE_TEXTURE_CYLINDER:
            tube = new TextureCylinder(numCenters, textureType);
            break;
        default:
            tube = new SimpleShapes(numCenters, SimpleShapes::CIRCLE_OF_SQUARES);
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
            // path class updates player
            path->update(player);
            break;
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

