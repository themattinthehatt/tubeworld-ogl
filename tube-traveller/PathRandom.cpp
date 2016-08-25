//
// Created by mattw on 8/24/16.
//

#include <iostream>
#include <random>
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PathRandom.h"

PathRandom::PathRandom(GLint numCenters,
                       const bool *keysPressed_,
                       const bool *keysToggled_) {

    keysPressed = keysPressed_;
    keysToggled = keysToggled_;

    // distance between slice centers
    spacing = 20.0f;

    // initial changes to changes in horizontal and vertical angles
    deltaHorzAngleUpdate = 0.f;
    deltaVertAngleUpdate = 0.f;

    // sets curvature (and min and max values) in horizontal direction
    horzAngleUpdateRange    = 0.0001f * spacing;
    minHorzAngleUpdateRange = 0.00001f * spacing;
    maxHorzAngleUpdateRange = 0.001f * spacing;

    // sets curvature (and min and max values) in vertical direction
    vertAngleUpdateRange    = 0.0001f;
    minVertAngleUpdateRange = 0.00001f;
    maxVertAngleUpdateRange = 0.01f;

    // set limits on actual delta angles
    maxDeltaHorzAngle = 0.1f;
    maxDeltaVertAngle = 0.1f;

    // set initial position/orientation
    positions.push_back(glm::vec3(0.f, 0.f, 0.f));
    horizontalAngles.push_back(PI/2);
    verticalAngles.push_back(PI/2);
    // update heading information.
    headings.push_back(glm::vec3(
            cos(horizontalAngles[0]) * sin(verticalAngles[0]),
            sin(horizontalAngles[0]) * sin(verticalAngles[0]),
            cos(verticalAngles[0]))
    );
    // right vector
    rights.push_back(glm::vec3(
            cos(horizontalAngles[0] - PI/2),
            sin(horizontalAngles[0] - PI/2),
            0)
    );
    // up vector
    ups.push_back(glm::cross(rights[0], headings[0]));


    // set up rng
    seed =  static_cast<unsigned long>(glfwGetTime());
    auto randNum = std::bind(std::uniform_real_distribution<GLfloat>(
            -horzAngleUpdateRange, horzAngleUpdateRange), std::mt19937(seed));

    // create rest of path
    deltaHorzAngleUpdate = 0.01;
    deltaVertAngleUpdate = 0.01;
    for (int i = 1; i < numCenters; ++i) {

        deltaHorzAngleUpdate += randNum();
        deltaVertAngleUpdate += randNum();
        horizontalAngles.push_back(horizontalAngles[i-1] + deltaHorzAngleUpdate);
        verticalAngles.push_back(verticalAngles[i-1] + deltaVertAngleUpdate);

        // update heading information.
        headings.push_back(glm::vec3(
                cos(horizontalAngles[i]) * sin(verticalAngles[i]),
                sin(horizontalAngles[i]) * sin(verticalAngles[i]),
                cos(verticalAngles[i]))
        );
        // right vector
        rights.push_back(glm::vec3(
                cos(horizontalAngles[i] - PI/2),
                sin(horizontalAngles[i] - PI/2),
                0)
        );
        // up vector
        ups.push_back(glm::cross(rights[i], headings[i]));

        positions.push_back(positions[i-1] + spacing * headings[i]);

    }
    firstElement = 0;
    lastElement = numCenters-1;

}


void PathRandom::update(Player &player) {

//        player.update();

    // increase speed additively or mutliplicatively
    if (keysPressed[GLFW_KEY_UP]) {
        if (keysPressed[GLFW_KEY_LEFT_SHIFT])
            speed = 1.01f*speed;
        else
            speed += 0.1f;
    }
    // decrease speed additively or mutliplicatively
    if (keysPressed[GLFW_KEY_DOWN]) {
        if (keysPressed[GLFW_KEY_LEFT_SHIFT])
            speed = 0.99f*speed;
        else
            speed -= 0.1f;
    }
    // increase vertical curvature
    if (keysPressed[GLFW_KEY_W]) {
        vertAngleUpdateRange += minVertAngleUpdateRange;
    }
    // decrease vertical curvature
    if (keysPressed[GLFW_KEY_S]) {
        vertAngleUpdateRange -= minVertAngleUpdateRange;
    }
    // increase horizontal curvature
    if (keysPressed[GLFW_KEY_D]) {
        horzAngleUpdateRange += minHorzAngleUpdateRange;
    }
    // decrease horizontal curvature
    if (keysPressed[GLFW_KEY_A]) {
        horzAngleUpdateRange -= minHorzAngleUpdateRange;
    }

    // check bounds on speed and curvature
    if (speed > maxSpeed) {
        speed = maxSpeed;
    } else if (speed < minSpeed) {
        speed = minSpeed;
    }
    if (vertAngleUpdateRange > maxVertAngleUpdateRange) {
        vertAngleUpdateRange = maxVertAngleUpdateRange;
    } else if (vertAngleUpdateRange < minVertAngleUpdateRange) {
        vertAngleUpdateRange = minVertAngleUpdateRange;
    }
    if (horzAngleUpdateRange > maxHorzAngleUpdateRange) {
        horzAngleUpdateRange = maxHorzAngleUpdateRange;
    } else if (horzAngleUpdateRange < minHorzAngleUpdateRange) {
        horzAngleUpdateRange = minHorzAngleUpdateRange;
    }


    // update time variables.
    player.setLastTime(player.getCurrentTime());
    player.setCurrentTime(static_cast<float>(glfwGetTime()));
    player.setDeltaTime(player.getCurrentTime() - player.getLastTime());

    // update distanceToTravel
    distanceToTravel = player.getDeltaTime() * speed;

    // move along segments
    segsToAdd = 0;
    while (distanceToTravel > distanceRemaining) {
        // move to next slice center
        distanceToTravel -= distanceRemaining;
        // calculate length of next curve segment
        distanceTotal = spacing;
        // reset distance remaining
        distanceRemaining = distanceTotal;
        // update number of segments to add
        segsToAdd++;
    }

    // update distance info
    distanceRemaining -= distanceToTravel;

    // update player information
    currElement = (firstElement + segsToAdd) % numCenters;
    nextElement = (currElement + 1) % numCenters;

    propOfPathTraveled = (distanceTotal - distanceRemaining) / distanceTotal;

    // linearly interp position, heading, right and up vectors for smoothness
    player.setPosition((1-propOfPathTraveled) * positions[currElement] +
                        propOfPathTraveled * positions[nextElement]);
    player.setHeading((1-propOfPathTraveled) * headings[currElement] +
                      propOfPathTraveled * headings[nextElement]);
    player.setRight((1-propOfPathTraveled) * rights[currElement] +
                    propOfPathTraveled * rights[nextElement]);
    player.setUp((1-propOfPathTraveled) * ups[currElement] +
                 propOfPathTraveled * ups[nextElement]);

    // update new segments
    for (int i = 0; i < segsToAdd; ++i) {

        auto randNum = std::bind(std::uniform_real_distribution<GLfloat>(
                -horzAngleUpdateRange, horzAngleUpdateRange),
                 std::mt19937(static_cast<unsigned long>(glfwGetTime())));

        deltaHorzAngleUpdate += randNum();
        deltaVertAngleUpdate += randNum();

        // limit size of changes in angle
        if (deltaHorzAngleUpdate > maxDeltaHorzAngle) {
            deltaHorzAngleUpdate = maxDeltaHorzAngle;
        } else if (deltaHorzAngleUpdate < -maxDeltaHorzAngle) {
            deltaHorzAngleUpdate = -maxDeltaHorzAngle;
        }
        if (deltaVertAngleUpdate > maxDeltaVertAngle) {
            deltaVertAngleUpdate = maxDeltaVertAngle;
        } else if (deltaVertAngleUpdate < -maxDeltaVertAngle) {
            deltaVertAngleUpdate = -maxDeltaVertAngle;
        }

        std::cout << deltaVertAngleUpdate << " " << deltaHorzAngleUpdate << std::endl;
        horizontalAngles[firstElement] = horizontalAngles[lastElement] +
                                         deltaHorzAngleUpdate;
        verticalAngles[firstElement] = verticalAngles[lastElement] +
                                         deltaVertAngleUpdate;

        // update heading information.
        headings[firstElement] = glm::vec3(
                cos(horizontalAngles[firstElement]) * sin(verticalAngles[firstElement]),
                sin(horizontalAngles[firstElement]) * sin(verticalAngles[firstElement]),
                cos(verticalAngles[firstElement])
        );
        // right vector
        rights[firstElement] = glm::vec3(
                cos(horizontalAngles[firstElement] - PI/2),
                sin(horizontalAngles[firstElement] - PI/2),
                0
        );
        // up vector
        ups[firstElement] = glm::cross(rights[firstElement], headings[firstElement]);
        positions[firstElement] = positions[lastElement] +
                                  spacing * headings[firstElement];

        // circle indices around
        lastElement = firstElement;
        firstElement = (firstElement + 1) % numCenters;
    }
}

void PathRandom::clean() { }
