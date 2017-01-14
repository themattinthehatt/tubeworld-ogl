//
// Created by mattw on 1/7/17.
//

#include <iostream>
#include <vector>
#include "MapNav.h"
//#include "../island-traveller/IslandTraveller.h"
//#include "../tube-traveller/TubeTraveller.h"

MapNav::MapNav() {

    // keep track of time changes
    timePrevious = 0.f;
    timeCurrent = glfwGetTime();
    timeDelta = 0.f;

    // island flags (most of this is handled by IslandTraveller class)
    renderIsland = true;
    stopIslandFlag = false;

    // tube flags/timers
    renderTubeFadeIn = false;
    renderTubeNormal = false;
    renderTubeFadeOut = false;
    renderTubeFinal = false;
    renderTube = false;
    tubeFadeInTime = 0.5f;      // sec
    tubeFadeOutTime = 0.5f;     // sec
    tubeTotalTime = 10.f;        // sec
    tubeTimeCumulative = 0.f;
    stopTubeFlag = false;

    // start rendering with island
    island = new IslandTraveller();
    tube = nullptr;

}

void MapNav::update(Camera &cam, Player &player) {

    timePrevious = timeCurrent;
    timeCurrent = glfwGetTime();
    timeDelta = timeCurrent - timePrevious;

    // if rendering island
    if (renderIsland) {
        if (!stopIslandFlag) {
            // render island normally
            stopIslandFlag = island->update(cam, player);
            island->draw();
        } else {
            // no rendering of island; clean up for transition to tube
            island->clean();
            delete island;
            island = nullptr;

            // reset island/tube flags/timers
            renderIsland = false;
            stopIslandFlag = false;
            renderTube = true;
            renderTubeFadeIn = true;


            // ----------------- initialize tube traveller ---------------------
            TubeTraveller::PathGeneratorType pathType =
                    TubeTraveller::PATH_RANDOM;
            TubeTraveller::TubeType tubeType =
                    TubeTraveller::TUBE_TEXTURE_CYLINDER_LIGHT;
            TextureCylinderLight::TextureType textureType =
                    TextureCylinderLight::TEXTURE_NOISE;
            TextureCylinderLight::LightStyle lightStyle =
                    TextureCylinderLight::LIGHTSTYLE_POINT;
            // Notes:
            // *TextureType only applies to TubeType of TUBE_TEXTURE_CYLINDER_LIGHT
            // *TEXTURE_BINARY and TEXTURE NOISE only work with PATH_RANDOM

            GLint numCenters = 100;
            tube = new TubeTraveller(numCenters, pathType,
                                    tubeType, textureType, lightStyle);

        }
    }

    // if rendering tube
    if (renderTube) {
        if (renderTubeFadeIn) {
            // rendering with fade in
            tubeTimeCumulative += timeDelta;
            if (tubeTimeCumulative > tubeFadeInTime) {
                tubeTimeCumulative = tubeFadeInTime;
                renderTubeFadeIn = false;
                renderTubeNormal = true;
            }
            tube->update(cam, player);
            tube->setFadeStep(tubeFadeInTime - tubeTimeCumulative);
            tube->setFadeTotal(tubeFadeInTime);
            tube->draw();
        } else if (renderTubeNormal) {
            // render tube normally
            tubeTimeCumulative += timeDelta;
            if (tubeTimeCumulative > tubeTotalTime - tubeFadeOutTime) {
                // transition to fade out phase
                renderTubeNormal = false;
                renderTubeFadeOut = true;
                // for easy fading
                tubeTimeCumulative = 0.f;
            }
            tube->update(cam, player);
            tube->draw();
        } else if (renderTubeFadeOut) {
            // rendering with fade out
            tubeTimeCumulative += timeDelta;
            if (tubeTimeCumulative > tubeFadeOutTime) {
                tubeTimeCumulative = tubeFadeOutTime;
                renderTubeFadeOut = false;
                renderTubeFinal = true;
            }
            tube->update(cam, player);
            tube->setFadeStep(tubeTimeCumulative);
            tube->setFadeTotal(tubeFadeOutTime);
            tube->draw();
        } else if (renderTubeFinal) {
            // final rendering of tube
            tube->update(cam, player);
            tube->draw();
            tube->clean();
            delete tube;
            tube = nullptr;

            // reset island/sketch flags/time
            tubeTimeCumulative = 0.f;
            renderTube = false;
            renderTubeFinal = false;
            renderIsland = true;

            // initialize island
            island = new IslandTraveller();

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