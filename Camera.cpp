//
// Created by mattw on 8/13/16.
//
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

/* Camera();
 * Constructor method for Camera class
 */
Camera::Camera(GLFWwindow *window_)
{
    // window for drawing
    window = window_;

    // start near origin facing along y-axis
    position0 = glm::vec3(0,-5,0);
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

    fov = 45.0f;

    Projection = glm::perspective(fov, 4.0f / 3.0f, 0.1f, 10000.0f);

}

/* ~Camera();
 * Destructor method for Camera class
 */
Camera::~Camera() {};

/* handleInput()
 * Handle user keypresses
 */
void Camera::handleInput()
{
    // move forward or up
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            position += up * deltaTime * speed;
        else
            position += heading * deltaTime * speed;
    }
    // move backward or down
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            position -= up * deltaTime * speed;
        else
            position -= heading * deltaTime * speed;
    }
    // strafe right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    // strafe left
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
    // rotate up
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        verticalAngle -= deltaTime * rotationSpeed;
    }
    // rotate down
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        verticalAngle += deltaTime * rotationSpeed;
    }
    // rotate right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngle -= deltaTime * rotationSpeed;
    }
    // rotate left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngle += deltaTime * rotationSpeed;
    }
    // decrease speed
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        speed -= 0.1;
    }
    // increase speed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        speed += 0.1;
    }
    // reset
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
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
 * updateCamera()
 * Update camera position in window
 */
void Camera::update()
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

    // update the view matrix.
    View = glm::lookAt(position, position + heading, up);

}

/*
 * resetCamera()
 * resets camera position to initial position given in constructor
 */
void Camera::reset()
{
    position = position0;
    horizontalAngle = horizontalAngle0;
    verticalAngle = verticalAngle0;
}