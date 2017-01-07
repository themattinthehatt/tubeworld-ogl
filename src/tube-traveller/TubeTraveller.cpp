//
// Created by mattw on 8/16/16.
//

#include <iostream>
#include "TubeTraveller.h"
#include "path-generators/PathCircle.h"
#include "path-generators/PathUserInput.h"
#include "path-generators/PathRandom.h"
#include "tube-generators/SimpleShapes.h"
#include "tube-generators/TextureCylinderLight.h"

TubeTraveller::TubeTraveller(GLint numCenters,
                             PathGeneratorType pathType,
                             TubeType tubeType,
                             TextureCylinderLight::TextureType textureType,
                             TextureCylinderLight::LightStyle lightStyle)
        : io(IOHandler::getInstance()){

    // create skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files = {"data/textures/box3/front.bmp",
                                      "data/textures/box3/back.bmp",
                                      "data/textures/box3/left.bmp",
                                      "data/textures/box3/right.bmp",
                                      "data/textures/box3/up.bmp",
                                      "data/textures/box3/down.bmp"};
    skybox = new Skybox(files, 1000.0f);

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
            tube = new SimpleShapes(numCenters, SimpleShapes::SQUARE_OF_SQUARES);
            break;
        case TUBE_CUBES_CIRC:
            tube = new SimpleShapes(numCenters, SimpleShapes::CIRCLE_OF_SQUARES);
            break;
        case TUBE_CYLINDER:
            tube = new SimpleShapes(numCenters, SimpleShapes::CYLINDER);
            break;
        case TUBE_TEXTURE_CYLINDER_LIGHT:
            tube = new TextureCylinderLight(numCenters, textureType, lightStyle);
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

    // update skybox
    skybox->update(cam);
}

void TubeTraveller::draw() {
    tube->draw();
    skybox->draw();
}

void TubeTraveller::clean() {
    skybox->clean();
    path->clean();
    tube->clean();
    delete skybox;
    delete path;
    delete tube;
}
