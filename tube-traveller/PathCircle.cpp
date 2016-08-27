//
// Created by mattw on 8/27/16.
//

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PathCircle.h"

PathCircle::PathCircle(GLint numCenters_) : io(IOHandler::getInstance()){

    // number of slices
    numCenters = numCenters_;

    // distance between slice centers
    spacing = 20.0f;

    // radius of circle
    radius = 100.f;

    GLfloat angle;
    for (int i = 0; i < numCenters; ++i) {

        angle = static_cast<GLfloat>(i) / numCenters * 2.f * PI;

        positions.push_back(glm::vec3(radius * cos(angle),
                                      radius * sin(angle),
                                      0));

        horizontalAngles.push_back(angle + PI/2);
        verticalAngles.push_back(PI/2);

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

    }
}


void PathCircle::update(Player &player) {

    // update time variables.
    player.setLastTime(player.getCurrentTime());
    player.setCurrentTime(static_cast<float>(glfwGetTime()));
    player.setDeltaTime(player.getCurrentTime() - player.getLastTime());
    player.moveTheta(player.getDeltaTime() * player.getRotationSpeed(),
                     glm::vec3(0.f, 0.f, 0.f));
    // move forward or up
    if (io.keysPressed[GLFW_KEY_UP]) {
        if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
            player.moveUp(player.getDeltaTime() * player.getSpeed());
        else
            player.moveTheta(player.getDeltaTime() * player.getRotationSpeed(),
                             glm::vec3(0.f, 0.f, 0.f));
    }
    // move backward or down
    if (io.keysPressed[GLFW_KEY_DOWN]) {
        if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
            player.moveDown(player.getDeltaTime() * player.getSpeed());
        else
            player.moveTheta(-player.getDeltaTime() * player.getRotationSpeed(),
                             glm::vec3(0.f, 0.f, 0.f));
    }
    // strafe right
    if (io.keysPressed[GLFW_KEY_RIGHT]) {
        player.moveRight(player.getDeltaTime() * player.getSpeed());
    }
    // strafe left
    if (io.keysPressed[GLFW_KEY_LEFT]) {
        player.moveLeft(player.getDeltaTime() * player.getSpeed());
    }
    // rotate up
    if (io.keysPressed[GLFW_KEY_W]) {
        player.rotateUp(player.getDeltaTime() * player.getRotationSpeed());
    }
    // rotate down
    if (io.keysPressed[GLFW_KEY_S]) {
        player.rotateDown(player.getDeltaTime() * player.getRotationSpeed());
    }
    // rotate right
    if (io.keysPressed[GLFW_KEY_D]) {
        player.rotateRight(player.getDeltaTime() * player.getRotationSpeed());
    }
    // rotate left
    if (io.keysPressed[GLFW_KEY_A]) {
        player.rotateLeft(player.getDeltaTime() * player.getRotationSpeed());
    }
    // decrease speed
    if (io.keysPressed[GLFW_KEY_E]) {
        player.incrementRotationSpeed(-0.001f);
    }
    // increase speed
    if (io.keysPressed[GLFW_KEY_R]) {
        player.incrementRotationSpeed(0.001f);
    }
    // reset
    if (io.keysPressed[GLFW_KEY_Q]) {
        player.reset();
    }

    // clamp vertical angle b/t 0 and PI
    if (player.getVerticalAngle() < 0)
        player.setVerticalAngle(0);
    else if (player.getVerticalAngle() > PI)
        player.setVerticalAngle(PI);

    // mod out horizontal angle by 2*PI
    player.setHorizontalAngle(static_cast<float>(
                                      fmod(player.getHorizontalAngle(), 2.0f * PI)));

    // restrict speed
    if (player.getSpeed() > player.getMaxSpeed())
        player.setSpeed(player.getMaxSpeed());
    else if (player.getSpeed() < 0)
        player.setSpeed(0);

    // update heading information.
    player.setHeading(glm::vec3(
            cos(player.getHorizontalAngle()) * sin(player.getVerticalAngle()),
            sin(player.getHorizontalAngle()) * sin(player.getVerticalAngle()),
            cos(player.getVerticalAngle()))
    );
    // right vector
    player.setRight(glm::vec3(
            cos(player.getHorizontalAngle() - PI/2),
            sin(player.getHorizontalAngle() - PI/2),
            0)
    );
    // up vector
    player.setUp(glm::cross(player.getRight(), player.getHeading()));

}

void PathCircle::clean() { }
