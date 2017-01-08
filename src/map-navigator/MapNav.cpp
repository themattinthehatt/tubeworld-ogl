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
    islandEndCount = 100;
    islandCounter = islandEndCount + 1;
    stopIslandFlag = false;
    island = new IslandTraveller();

    // set up tube info
    tubeEndCount = 2000; // sec
    tubeCounter = 0;

}

void MapNav::update(Camera &cam, Player &player) {

    if (islandCounter > 0) {
        if (islandCounter > islandEndCount) {
            stopIslandFlag = island->update(cam, player);
            island->draw();
            if (stopIslandFlag) {
                islandCounter = islandEndCount;
            }
        } else if (islandCounter > 1) {
            islandCounter--;
            island->update(cam, player);
            island->draw();
        } else if (islandCounter == 1) {
            // final rendering of island
            islandCounter--;
            island->update(cam, player);
            island->draw();
            island->clean();
            delete island;

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
        } else if (tubeCounter == 1) {
            // final rendering of tube
            tubeCounter--;
            tube->update(cam, player);
            tube->draw();
            tube->clean();
            delete tube;

            // initialize island
            island = new IslandTraveller();
            islandCounter = islandEndCount + 1;

            // reset player position
            player.setPosition(glm::vec3(0,0,0));
            player.setHorizontalAngle(PI/2);
            player.setVerticalAngle(PI/2);
            player.setHeading(glm::vec3(
                    cos(player.getHorizontalAngle()) * sin(player.getVerticalAngle()),
                    sin(player.getHorizontalAngle()) * sin(player.getVerticalAngle()),
                    cos(player.getVerticalAngle())));
            player.setRight(glm::vec3(
                    cos(player.getHorizontalAngle() - PI/2),
                    sin(player.getHorizontalAngle() - PI/2),
                    0));
            player.setUp(glm::cross(player.getRight(), player.getHeading()));
            player.setSpeed(10.f);
            player.setRotationSpeed(1.0f);

        }
    }
}

void MapNav::draw() {

}

void MapNav::clean() {

}