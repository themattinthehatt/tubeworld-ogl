//
// Created by mattw on 8/13/16.
//

#ifndef PLAYGROUND_CAMERA_H
#define PLAYGROUND_CAMERA_H

#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const float PI = 3.141592653589f;

class Camera
{
private:

    glm::vec3 position0;                    // initial position vector
    glm::vec3 position;                     // position vector
    glm::vec3 heading;                      // heading vector
    glm::vec3 right;                        // vector pointing to right
    glm::vec3 up;                           // vector pointing up

    float horizontalAngle0;                 // initial horizontal angle
    float horizontalAngle;                  // horizontal angle in radians
    float verticalAngle0;                   // vertical angle in radians
    float verticalAngle;                    // vertical angle in radians

    const float maxSpeed = 80.f;            // max pan speed
    const float maxRotationSpeed = 3.0f;    // max rotation speed
    float speed;                            // pan speed
    float rotationSpeed;                    // rotation speed

    float lastTime;                         // time at last update
    float currentTime;                      // time at current update
    float deltaTime;                        // time between current and previous

    float fov;                              // field of view, in degrees

    GLFWwindow *window;                     // window to draw in

public:

    glm::mat4 View;                         // view matrix
    glm::mat4 Projection;                   // projection matrix

    Camera(GLFWwindow *window);             // constructor
    ~Camera();                              // destructor
    void handleInput();                     // interface with keyboard
    void update();                          // move camera
    void reset();                           // reset camera

};


#endif //PLAYGROUND_CAMERA_H
