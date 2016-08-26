//
// Created by mattw on 8/20/16.
//

#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Player.h"

/* Player();
 * Constructor method for Player class
 */
Player::Player() : io(IOHandler::getInstance())
{

    // start near origin facing along y-axis
    position0 = glm::vec3(0,0,0);
    horizontalAngle0 = PI/2;
    verticalAngle0 = PI/2;

    position = position0;
    horizontalAngle = horizontalAngle0;
    verticalAngle = verticalAngle0;

    // update heading information.
    heading = glm::vec3(
            cos(horizontalAngle) * sin(verticalAngle),
            sin(horizontalAngle) * sin(verticalAngle),
            cos(verticalAngle)
    );
    // right vector
    right = glm::vec3(
            cos(horizontalAngle - PI/2),
            sin(horizontalAngle - PI/2),
            0
    );
    // up vector
    up = glm::cross(right, heading);

    speed = 10.0f;
    rotationSpeed = 1.0f;

    currentTime = static_cast<float>(glfwGetTime());
}

/* ~Player();
 * Destructor method for Player class
 */
Player::~Player() {};

/* handleInput()
 * Handle user keypresses
 */
void Player::handleInput()
{
    // move forward or up
    if (io.keysPressed[GLFW_KEY_UP]) {
        if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
            moveUp(deltaTime * speed);
        else
            moveForward(deltaTime * speed);
    }
    // move backward or down
    if (io.keysPressed[GLFW_KEY_DOWN]) {
        if (io.keysPressed[GLFW_KEY_LEFT_SHIFT])
            moveDown(deltaTime * speed);
        else
            moveBackward(deltaTime * speed);
    }
    // strafe right
    if (io.keysPressed[GLFW_KEY_RIGHT]) {
        moveRight(deltaTime * speed);
    }
    // strafe left
    if (io.keysPressed[GLFW_KEY_LEFT]) {
        moveLeft(deltaTime * speed);
    }
    // rotate up
    if (io.keysPressed[GLFW_KEY_W]) {
        rotateUp(deltaTime * rotationSpeed);
    }
    // rotate down
    if (io.keysPressed[GLFW_KEY_S]) {
        rotateDown(deltaTime * rotationSpeed);
    }
    // rotate right
    if (io.keysPressed[GLFW_KEY_D]) {
        rotateRight(deltaTime * rotationSpeed);
    }
    // rotate left
    if (io.keysPressed[GLFW_KEY_A]) {
        rotateLeft(deltaTime * rotationSpeed);
    }
    // decrease speed
    if (io.keysPressed[GLFW_KEY_E]) {
        speed -= 0.1;
    }
    // increase speed
    if (io.keysPressed[GLFW_KEY_R]) {
        speed += 0.1;
    }
    // reset
    if (io.keysPressed[GLFW_KEY_Q]) {
        reset();
    }

    // clamp vertical angle b/t 0 and PI
    if (verticalAngle < 0)
        verticalAngle = 0;
    else if (verticalAngle > PI)
        verticalAngle = PI;

    // mod out horizontal angle by 2*PI
    horizontalAngle = static_cast<float>(fmod(horizontalAngle, 2.0f * PI));

    // restrict speed
    if (speed > maxSpeed)
        speed = maxSpeed;
    else if (speed < 0)
        speed = 0;
}

/*
 * updatePlayer()
 * Update Player position in window
 */
void Player::update()
{
    // update time variables.
    lastTime = currentTime;
    currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastTime;

    // handle input.
    handleInput();

    // update heading information.
    heading = glm::vec3(
            cos(horizontalAngle) * sin(verticalAngle),
            sin(horizontalAngle) * sin(verticalAngle),
            cos(verticalAngle)
    );
    // right vector
    right = glm::vec3(
            cos(horizontalAngle - PI/2),
            sin(horizontalAngle - PI/2),
            0
    );
    // up vector
    up = glm::cross(right, heading);
}

/*
 * resetPlayer()
 * resets Player position to initial position given in constructor
 */
void Player::reset()
{
    position = position0;
    horizontalAngle = horizontalAngle0;
    verticalAngle = verticalAngle0;
}

// movement functions
void Player::moveForward(GLfloat distance_) {
    position += heading * distance_;
}
void Player::moveUp(GLfloat distance_) {
    position += up * distance_;
}
void Player::moveBackward(GLfloat distance_) {
    position -= heading * distance_;
}
void Player::moveDown(GLfloat distance_) {
    position -= up * distance_;
}
void Player::moveRight(GLfloat distance_) {
    position += right * distance_;
}
void Player::moveLeft(GLfloat distance_) {
    position -= right * distance_;
}
void Player::rotateUp(GLfloat rotationAngle_) {
    verticalAngle -= rotationAngle_; // we're in standard spherical coords
}
void Player::rotateDown(GLfloat rotationAngle_) {
    verticalAngle += rotationAngle_;
}
void Player::rotateRight(GLfloat rotationAngle_) {
    horizontalAngle -= rotationAngle_;
}
void Player::rotateLeft(GLfloat rotationAngle_) {
    horizontalAngle += rotationAngle_;
}
void Player::moveTheta(GLfloat rotationAngle, glm::vec3 origin) {
    glm::mat4 model;
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    position = origin + glm::vec3(model * glm::vec4(position-origin, 1.0f));
//    heading = glm::vec3(model * glm::vec4(heading, 0.0f));
    horizontalAngle += rotationAngle;
}