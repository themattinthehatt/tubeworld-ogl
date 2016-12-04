//
// Created by mattw on 8/21/16.
//

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PathUserInput.h"

PathUserInput::PathUserInput(GLint numCenters_, GLfloat spacing_)
        :
        io(IOHandler::getInstance()){

    // number of slices
    numCenters = numCenters_;

    // distance between slice centers
    spacing = spacing_;

    // set up initial tube orientations
    for (int i = 0; i < numCenters; ++i) {
        positions.push_back(glm::vec3(0.f, 0.f, 0.f));
        horizontalAngles.push_back(PI/2);
        verticalAngles.push_back(PI/2);
        headings.push_back(glm::vec3(0.f,1.f, 0.f));
        ups.push_back(glm::vec3(0.f, 0.f, 1.f));
        rights.push_back(glm::vec3(1.f, 0.f, 0.f));
    }
}


void PathUserInput::update(Player &player) {
    GLfloat distance;

    // -----------------------------------------------------------------
    //                     Update player position
    // -----------------------------------------------------------------

    // update time variables.
    player.setLastTime(player.getCurrentTime());
    player.setCurrentTime(static_cast<float>(glfwGetTime()));
    player.setDeltaTime(player.getCurrentTime() - player.getLastTime());

    // always move forward
    distance = player.getDeltaTime() * player.getSpeed();
    deltaHorzAngle = player.getDeltaTime() * player.getRotationSpeed();
    deltaVertAngle = player.getDeltaTime() * player.getRotationSpeed();
    player.moveForward(0.1);

    // move forward or up
    if (io.keysPressed[GLFW_KEY_UP]) {
        if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
            player.moveUp(distance);
        else
            speed += 0.01f;
    }
    // move backward or down
    if (io.keysPressed[GLFW_KEY_DOWN]) {
        if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
            player.moveDown(distance);
        else
            speed -= 0.01f;
    }
    // strafe right
    if (io.keysPressed[GLFW_KEY_RIGHT]) {
        player.moveRight(distance);
    }
    // strafe left
    if (io.keysPressed[GLFW_KEY_LEFT]) {
        player.moveLeft(distance);
    }
    // rotate up
    if (io.keysPressed[GLFW_KEY_W]) {
        // don't instantaneously update angle; let it grow slowly
        growthUpVertAngle *= 0.9f;
        player.rotateUp((1 - growthUpVertAngle) * deltaVertAngle);
        // any decay will start from this same angle
        decayUpVertAngle = (1 - growthUpVertAngle);
    } else {
        // key not pressed; reset growth multiplier
        growthUpVertAngle = 1.0f;
        // decay
        decayUpVertAngle *= 0.95;
    }
    // rotate down
    if (io.keysPressed[GLFW_KEY_S]) {
        growthDownVertAngle *= 0.9f;
        player.rotateDown((1 - growthDownVertAngle) * deltaVertAngle);
        decayDownVertAngle = (1 - growthDownVertAngle);
    } else {
        growthDownVertAngle = 1.0f;
        decayDownVertAngle *= 0.95;
    }
    // rotate right
    if (io.keysPressed[GLFW_KEY_D]) {
        growthRightHorzAngle *= 0.9f;
        player.rotateRight((1 - growthRightHorzAngle) * deltaHorzAngle);
        decayRightHorzAngle = (1 - growthRightHorzAngle);
    } else {
        growthRightHorzAngle = 1.0f;
        decayRightHorzAngle *= 0.95;
    }
    // rotate left
    if (io.keysPressed[GLFW_KEY_A]) {
        growthLeftHorzAngle *= 0.9f;
        player.rotateLeft((1 - growthLeftHorzAngle) * deltaHorzAngle);
        decayLeftHorzAngle = (1 - growthLeftHorzAngle);
    } else {
        growthLeftHorzAngle = 1.0f;
        decayLeftHorzAngle *= 0.95;
    }
    // decrease speed
    if (io.keysPressed[GLFW_KEY_E]) {
        player.incrementSpeed(-0.1f);
    }
    // increase speed
    if (io.keysPressed[GLFW_KEY_R]) {
        player.incrementSpeed(0.1f);
    }
    // reset
    if (io.keysPressed[GLFW_KEY_Q]) {
        player.reset();
    }

    // -----------------------------------------------------------------
    //                     Update player info
    // -----------------------------------------------------------------

    // clamp vertical angle b/t 0 and PI
    if (player.getVerticalAngle() < 0)
        player.setVerticalAngle(0);
    else if (player.getVerticalAngle() > PI)
        player.setVerticalAngle(PI);

    // mod out horizontal angle by 2*PI
    player.setHorizontalAngle(static_cast<float>(
                                      fmod(player.getHorizontalAngle(),
                                           2.0f * PI)));

    // restrict speed
    if (player.getSpeed() > player.getMaxSpeed())
        player.setSpeed(player.getMaxSpeed());
    else if (player.getSpeed() < 0)
        player.setSpeed(0);

    // update heading information.
    player.setHeading(glm::vec3(
            cos(player.getHorizontalAngle()) *
            sin(player.getVerticalAngle()),
            sin(player.getHorizontalAngle()) *
            sin(player.getVerticalAngle()),
            cos(player.getVerticalAngle()))
    );
    // right vector
    player.setRight(glm::vec3(
            cos(player.getHorizontalAngle() - PI / 2),
            sin(player.getHorizontalAngle() - PI / 2),
            0)
    );
    // up vector
    player.setUp(glm::cross(player.getRight(), player.getHeading()));

    // -----------------------------------------------------------------
    //                          Update path
    // -----------------------------------------------------------------

    float temp = static_cast<float>(fmod(glfwGetTime() * speed,
                                    std::max(distance, 2*spacing)));
    // position[0] and position[1] are extrapolated from current player
    // position; all further position points are extrapolated from
    // position[1] and user input
    positions[0] = player.getPosition() - player.getHeading() * temp;
    horizontalAngles[0] = player.getHorizontalAngle();
    verticalAngles[0] = player.getVerticalAngle();
    headings[0] = player.getHeading();
    ups[0] = player.getUp();
    rights[0] = player.getRight();

    positions[1] = player.getPosition() + player.getHeading() *
                              (std::max(distance, 2*spacing) - temp);
    horizontalAngles[1] = player.getHorizontalAngle();
    verticalAngles[1] = player.getVerticalAngle();
    headings[1] = player.getHeading();
    ups[1] = player.getUp();
    rights[1] = player.getRight();

    for (int i = 2; i < numCenters; ++i) {

        // always move forward
        positions[i] = positions[i - 1] + headings[i - 1] *
                                          std::max(distance, 2 * spacing);
        verticalAngles[i] = verticalAngles[i - 1];
        horizontalAngles[i] = horizontalAngles[i - 1];

        // move forward or up
        if (io.keysPressed[GLFW_KEY_UP]) {
            if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
                positions[i] = positions[i] + ups[i - 1] * distance;
            else
                positions[i] = positions[i] + headings[i - 1] * distance;
        }
        // move backward or down
        if (io.keysPressed[GLFW_KEY_DOWN]) {
            if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
                positions[i] = positions[i] - ups[i - 1] * distance;
            else
                positions[i] = positions[i] - headings[i - 1] * distance;
        }
        // strafe right
        if (io.keysPressed[GLFW_KEY_RIGHT]) {
            positions[i] = positions[i] + rights[i - 1] * distance;
        }
        // strafe left
        if (io.keysPressed[GLFW_KEY_LEFT]) {
            positions[i] = positions[i] - rights[i - 1] * distance;
        }
        // rotate up
        if (io.keysPressed[GLFW_KEY_W]) {
            verticalAngles[i] = verticalAngles[i] -
                                (1 - growthUpVertAngle) * deltaVertAngle;
        } else {
            verticalAngles[i] = verticalAngles[i] -
                                decayUpVertAngle * deltaVertAngle;
        }
        // rotate down
        if (io.keysPressed[GLFW_KEY_S]) {
            verticalAngles[i] = verticalAngles[i] +
                                (1 - growthDownVertAngle) * deltaVertAngle;
        } else {
            verticalAngles[i] = verticalAngles[i] +
                                decayDownVertAngle * deltaVertAngle;
        }
        // rotate right
        if (io.keysPressed[GLFW_KEY_D]) {
            horizontalAngles[i] = horizontalAngles[i] -
                                  (1 - growthRightHorzAngle) * deltaHorzAngle;
        } else {
            horizontalAngles[i] = horizontalAngles[i] -
                                  decayRightHorzAngle * deltaHorzAngle;
        }
        // rotate left
        if (io.keysPressed[GLFW_KEY_A]) {
            horizontalAngles[i] = horizontalAngles[i] +
                                  (1 - growthLeftHorzAngle) * deltaHorzAngle;
        } else {
            horizontalAngles[i] = horizontalAngles[i] +
                                  decayLeftHorzAngle * deltaHorzAngle;
        }

        // clamp vertical angle b/t 0 and PI
//        if (verticalAngles[i] < 0)
//            verticalAngles[i] = 0;
//        else if (verticalAngles[i] > PI)
//            verticalAngles[i] = PI;

        // mod out horizontal angle by 2*PI
        horizontalAngles[i] = static_cast<float>(
                fmod(horizontalAngles[i], 2.0f * PI));


        // update heading information.
        headings[i] = glm::vec3(
                cos(horizontalAngles[i]) * sin(verticalAngles[i]),
                sin(horizontalAngles[i]) * sin(verticalAngles[i]),
                cos(verticalAngles[i])
        );
        // right vector
        rights[i] = glm::vec3(
                cos(horizontalAngles[i] - PI / 2),
                sin(horizontalAngles[i] - PI / 2),
                0
        );
        // up vector
        ups[i] = glm::cross(rights[i], headings[i]);

    }
}

void PathUserInput::clean() { }