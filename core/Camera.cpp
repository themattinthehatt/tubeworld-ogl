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
Camera::Camera()
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

    fov = 45.0f;

    Projection = glm::perspective(fov, 4.0f / 3.0f, 0.1f, 10000.0f);
}

/* ~Camera();
 * Destructor method for Camera class
 */
Camera::~Camera() {};

/*
 * updateCamera()
 * Update camera position in window
 */
void Camera::update(const Player &player)
{
    position = player.getPosition();
    heading = player.getHeading();
    right = player.getRight();
    up = player.getUp();

    // update the view matrix.
    View = glm::lookAt(position, position + heading, up);
}