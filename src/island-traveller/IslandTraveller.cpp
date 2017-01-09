//
// Created by mattw on 1/7/17.
//

#include <iostream>
#include <vector>
#include "IslandTraveller.h"
#include "perlin-block/PerlinBlock.h"

IslandTraveller::IslandTraveller() : io(IOHandler::getInstance()) {

    islandEndCount = 100;
    islandCounter = islandEndCount + 1; // start rendering with island
    stopIslandFlag = false;

    sketchEndCount = 100;
    sketchCounter = 0;
    stopSketchFlag = false;

    // start rendering with island
    island = new PerlinBlock();
    sketch = nullptr;

}

bool IslandTraveller::update(Camera &cam, Player &player) {

    // if rendering island
    if (islandCounter > 0) {
        if (islandCounter > islandEndCount) {
            // render island normally
            stopIslandFlag = island->update(cam, player);
            island->draw();
            // look for return flag
            if (stopIslandFlag > 1) {
                // begin transition to sketch
                islandCounter = islandEndCount;
                // if stopIslandFlag == 0, do nothing
                // if stopIslandFlag == 1, return true below
            }
        } else if (islandCounter > 1) {
            // rendering with fade
            islandCounter--;
            island->update(cam, player);
            island->draw();
        } else if (islandCounter == 1) {
            // final rendering of island
            islandCounter--; // now equal to zero
            island->update(cam, player);
            island->draw();
            island->clean();
            delete island;
            island = nullptr;

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
            sketchCounter = sketchEndCount + 1;

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

    // if rendering sketch
    if (sketchCounter > 0) {
        if (sketchCounter > sketchEndCount) {
            stopSketchFlag = sketch->update(cam, player);
            sketch->draw();
            if (stopSketchFlag > 0) {
                // begin transition to island
                sketchCounter = sketchEndCount;
                // if stopIslandFlag == 0, do nothing
            }
        } else if (sketchCounter > 1) {
            sketchCounter--;
            // dim lighting
            sketch->update(cam, player);
            sketch->draw();
        } else if (sketchCounter == 1) {
            // final rendering of sketch
            sketchCounter--; // now equal to zero
            sketch->update(cam, player);
            sketch->draw();
            sketch->clean();
            delete sketch;
            sketch = nullptr;

            // initialize island
            island = new PerlinBlock();
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

    if (stopIslandFlag == 1) {
        return true;
    } else {
        return false;
    }

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