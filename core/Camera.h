//
// Created by mattw on 8/13/16.
//

#ifndef PLAYGROUND_CAMERA_H
#define PLAYGROUND_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Player.h"

class Camera
{
private:

    glm::vec3 position0;                    // initial position vector
    glm::vec3 position;                     // position vector
    glm::vec3 heading;                      // heading vector
    glm::vec3 right;                        // vector pointing to right
    glm::vec3 up;                           // vector pointing up

    GLfloat horizontalAngle0;               // initial horizontal angle
    GLfloat horizontalAngle;                // horizontal angle in radians
    GLfloat verticalAngle0;                 // vertical angle in radians
    GLfloat verticalAngle;                  // vertical angle in radians

    GLfloat fov;                            // field of view, in degrees

    glm::mat4 View;                         // view matrix
    glm::mat4 Projection;                   // projection matrix

public:

    Camera();                               // constructor
    ~Camera();                              // destructor
    void update(const Player &player);      // move camera

    // getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getPosition0() const { return position0; }
    glm::mat4 getView() const { return View; }
    glm::mat4 getProjection() const { return Projection; }

};


#endif //PLAYGROUND_CAMERA_H