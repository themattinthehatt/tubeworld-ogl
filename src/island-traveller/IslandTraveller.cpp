//
// Created by mattw on 1/7/17.
//

#include <iostream>
#include <vector>
#include "IslandTraveller.h"

IslandTraveller::IslandTraveller() : io(IOHandler::getInstance()) {

    // create skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files = {"data/textures/sky/front.bmp",
                                      "data/textures/sky/back.bmp",
                                      "data/textures/sky/left.bmp",
                                      "data/textures/sky/right.bmp",
                                      "data/textures/sky/up.bmp",
                                      "data/textures/sky/down.bmp"};
    skybox = new Skybox(files, 1000.0f);

    /*
     * front -> back, rotate cw x 1
     * back -> front, rotate ccw x 1
     * left - fine
     * right - ccw x 2
     * up - cw x 1
     * down - cw x 1
     */

    island = new Island();

}

bool IslandTraveller::update(Camera &cam, Player &player) {

    // update player
    player.update();

    // compute view and projection matrices from player info
    cam.update(player);

    // update island
    island->update(cam, player);

    // update skybox
    skybox->update(cam);

    // decide if tube entry is valid
    glm::vec3 posPlayer = player.getPosition();
    glm::vec3 posPortal = glm::vec3(2.f, 10.f, 2.f);
    GLfloat dist = glm::distance(posPlayer, posPortal);
    std::cout << dist << std::endl;
    if (dist < 2.f) {
        return true;
    } else {
        return false;
    }

}

void IslandTraveller::draw() {
    island->draw();
    skybox->draw();
}

void IslandTraveller::clean() {
    skybox->clean();
    island->clean();
    delete skybox;
    delete island;
}