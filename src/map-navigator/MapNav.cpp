//
// Created by mattw on 1/7/17.
//

#include <iostream>
#include <vector>
#include "MapNav.h"
//#include "../island-traveller/IslandTraveller.h"
//#include "../tube-traveller/TubeTraveller.h"

MapNav::MapNav() {

    // set up island info
    islandBegCount = 300;
    islandEndCount = 300;
    islandCounter = islandEndCount + islandBegCount + 1;
    stopIslandFlag = false;

    // set up tube info
    tubeEndCount = 1000; // TODO sec
    tubeCounter = 0;

    // start rendering with island
    island = new IslandTraveller();
    tube = nullptr;

}

void MapNav::update(Camera &cam, Player &player) {

    // if rendering island
    if (islandCounter > 0) {
        if (islandCounter > islandEndCount + 1) {
            // rendering with fade in
            islandCounter--;
            island->update(cam, player);
            island->draw();
        }
        if (islandCounter > islandEndCount) {
            // render island normally
            stopIslandFlag = island->update(cam, player);
            island->draw();
            // look for return flag
            if (stopIslandFlag) {
                // begin transition to sketch
                islandCounter = islandEndCount;
            }
        } else if (islandCounter > 1) {
            // rendering with fade out
            islandCounter--;
            // dim island
            island->update(cam, player);
            island->draw();
        } else if (islandCounter == 1) {
            // no rendering of island
            islandCounter--; // now equal to zero
            // delete island
            island->clean();
            delete island;
            island = nullptr;

            // initialize tube traveller
            //    TubeTraveller::PathGeneratorType pathType =
            //          TubeTraveller::PATH_CIRCLE;
            //    TubeTraveller::TubeType tubeType =
            //          TubeTraveller::TUBE_CUBES_SQ;

            TubeTraveller::PathGeneratorType pathType =
                    TubeTraveller::PATH_RANDOM;
            TubeTraveller::TubeType tubeType =
                    TubeTraveller::TUBE_TEXTURE_CYLINDER_LIGHT;

            // Notes
            // TextureType only applies to a TubeType of TUBE_TEXTURE_CYLINDER
            // TEXTURE_BINARY and TEXTURE NOISE only work with PATH_RANDOM
            //    TextureCylinderLight::TextureType textureType =
            //            TextureCylinderLight::TEXTURE_FILES_STATIC;
            //    TextureCylinderLight::TextureType textureType =
            //            TextureCylinderLight::TEXTURE_RAINBOW;
            //    TextureCylinderLight::TextureType textureType =
            //            TextureCylinderLight::TEXTURE_BINARY;
            TextureCylinderLight::TextureType textureType =
                    TextureCylinderLight::TEXTURE_NOISE;

            TextureCylinderLight::LightStyle lightStyle =
                    TextureCylinderLight::LIGHTSTYLE_POINT;

            GLint numCenters = 100;
            tube = new TubeTraveller(numCenters, pathType,
                                    tubeType, textureType, lightStyle);
            tubeCounter = tubeEndCount;
        }
    }

    // if rendering tube
    if (tubeCounter > 0) {
        if (tubeCounter > tubeEndCount) {
            tubeCounter--;
            tube->update(cam, player);
            tube->draw();
        } else if (tubeCounter > 1) {
            tubeCounter--;
            // dim lighting
            tube->update(cam, player);
            tube->draw();
        } else if (tubeCounter ==1) {
            tubeCounter--; // now equal to zero
            // final rendering of tube
            tube->update(cam, player);
            tube->draw();
            tube->clean();
            delete tube;
            tube = nullptr;


            // initialize island
            island = new IslandTraveller();
            islandCounter = islandEndCount + 1;

            // set player position
            glm::vec3 position = glm::vec3(0,0,0);
            GLfloat horizontalAngle = PI/2;
            GLfloat verticalAngle = PI/2;
            GLfloat speed = 10.f;
            GLfloat rotSpeed = 1.f;
            player.setAttributes(position, horizontalAngle, verticalAngle,
                                 speed, rotSpeed);

        }
    }
}

void MapNav::draw() {

}

void MapNav::clean() {
    if (island) {
        island->clean();
    }
    delete island;
    if (tube) {
        tube->clean();
    }
    delete tube;
}