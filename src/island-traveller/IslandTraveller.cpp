//
// Created by mattw on 1/7/17.
//

#include <iostream>
#include <vector>
#include "IslandTraveller.h"
#include "perlin-block/PerlinBlock.h"

IslandTraveller::IslandTraveller() : io(IOHandler::getInstance()) {

    // keep track of time changes
    timePrevious = 0.f;
    timeCurrent = glfwGetTime();
    timeDelta = 0.f;

    stopIslandTraveller = false; // to return to MapNav class

    // island flags/timers
    renderIslandFadeIn = true;
    renderIslandNormal = false;
    renderIslandFadeOut = false;
    renderIslandFinal = false;
    renderIsland = true;
    islandFadeInTime = 0.5f;     // sec
    islandFadeOutTime = 0.5f;    // sec
    islandTimeCumulative = 0.f;
    stopIslandFlag = false;

    // sketch flags/timers
    renderSketchFadeIn = false;
    renderSketchNormal = false;
    renderSketchFadeOut = false;
    renderSketchFinal = false;
    renderSketch = false;
    sketchFadeInTime = 0.5f;     // sec
    sketchFadeOutTime = 0.5f;   // sec
    sketchTimeCumulative = 0.f;
    stopSketchFlag = false;

    // start rendering with island
    island = new PerlinBlock();
    sketch = nullptr;

}

bool IslandTraveller::update(Camera &cam, Player &player) {

    timePrevious = timeCurrent;
    timeCurrent = glfwGetTime();
    timeDelta = timeCurrent - timePrevious;

    // if rendering island
    if (renderIsland) {
        if (renderIslandFadeIn) {
            // rendering with fade in
            islandTimeCumulative += timeDelta;
            if (islandTimeCumulative > islandFadeInTime) {
                islandTimeCumulative = islandFadeInTime;
                renderIslandFadeIn = false;
                renderIslandNormal = true;
            }
            island->update(cam, player);
            island->setFadeStep(islandFadeInTime - islandTimeCumulative);
            island->setFadeTotal(islandFadeInTime);
            island->draw();
        } else if (renderIslandNormal) {
            // render island normally
            stopIslandFlag = island->update(cam, player);
            island->draw();
            // look for return flag
            if (stopIslandFlag >= 1) {
                // begin transition to sketch
                renderIslandNormal = false;
                renderIslandFadeOut = true;
                islandTimeCumulative = 0.f;
            }
        } else if (renderIslandFadeOut) {
            // rendering with fade out; no player movement
            islandTimeCumulative += timeDelta;
            if (islandTimeCumulative > islandFadeOutTime) {
                islandTimeCumulative = islandFadeOutTime;
                renderIslandFadeOut = false;
                renderIslandFinal = true;
            }
            island->setFadeStep(islandTimeCumulative);
            island->setFadeTotal(islandFadeOutTime);
            island->draw();
        } else if (renderIslandFinal) {
            // no actual rendering; clean up for transition to sketch
            island->clean();
            delete island;
            island = nullptr;

            // reset island flags/time
            islandTimeCumulative = 0.f;
            renderIsland = false;
            renderIslandFinal = false;

            if (stopIslandFlag == 1) {
                // return to MapNav
                stopIslandTraveller = true;
            } else {
                renderSketch = true;
                renderSketchFadeIn = true;
                // initialize sketch
                switch (stopIslandFlag) {
                    case 10:
                        sketch = new Sketch01(0);
                        break;
                    case 11:
                        sketch = new Sketch01(1);
                        break;
                    case 12:
                        sketch = new Sketch01(2);
                        break;
                    default:
                        sketch = new Sketch01(0);
                }

                // set player position
                glm::vec3 position = glm::vec3(0, 0, 0);
                GLfloat horizontalAngle = PI / 2;
                GLfloat verticalAngle = PI / 2;
                GLfloat speed = 10.f;
                GLfloat rotSpeed = 1.f;
                player.setAttributes(position, horizontalAngle, verticalAngle,
                                     speed, rotSpeed);
            }
        }
    }

    // if rendering sketch
    if (renderSketch) {
        if (renderSketchFadeIn) {
            // rendering with fade in
            sketchTimeCumulative += timeDelta;
            if (sketchTimeCumulative > sketchFadeInTime) {
                sketchTimeCumulative = sketchFadeInTime;
                renderSketchFadeIn = false;
                renderSketchNormal = true;
            }
            sketch->update(cam, player);
            sketch->setFadeStep(sketchFadeInTime - sketchTimeCumulative);
            sketch->setFadeTotal(sketchFadeInTime);
            sketch->draw();
        } else if (renderSketchNormal) {
            // render sketch normally
            stopSketchFlag = sketch->update(cam, player);
            sketch->draw();
            // look for return flag
            if (stopSketchFlag > 0) {
                // begin transition to island
                renderSketchNormal = false;
                renderSketchFadeOut = true;
                sketchTimeCumulative = 0.f;
            }
        } else if (renderSketchFadeOut) {
            // rendering with fade out; no player movement
            sketchTimeCumulative += timeDelta;
            if (sketchTimeCumulative > sketchFadeOutTime) {
                sketchTimeCumulative = sketchFadeOutTime;
                renderSketchFadeOut = false;
                renderSketchFinal = true;
            }
            sketch->setFadeStep(sketchTimeCumulative);
            sketch->setFadeTotal(sketchFadeOutTime);
            sketch->draw();
        } else if (renderSketchFinal) {
            // final rendering of sketch
            sketch->update(cam, player);
            sketch->draw();
            sketch->clean();
            delete sketch;
            sketch = nullptr;

            // reset island/sketch flags/time
            sketchTimeCumulative = 0.f;
            renderSketch = false;
            renderSketchFinal = false;
            renderIsland = true;
            renderIslandFadeIn = true;

            // initialize island
            island = new PerlinBlock();

            // set player position
            glm::vec3 position = glm::vec3(0, 0, 0);
            GLfloat horizontalAngle = PI / 2;
            GLfloat verticalAngle = PI / 2;
            GLfloat speed = 10.f;
            GLfloat rotSpeed = 1.f;
            player.setAttributes(position, horizontalAngle, verticalAngle,
                                 speed, rotSpeed);

        }
    }

    return stopIslandTraveller;

}

void IslandTraveller::draw() {
}

void IslandTraveller::clean() {
    if (island) {
        island->clean();
    }
    delete island;
    if (sketch) {
        sketch->clean();
    }
    delete sketch;
}