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

    GLfloat horizontalAngle0;               // initial horizontal angle
    GLfloat horizontalAngle;                // horizontal angle in radians
    GLfloat verticalAngle0;                 // vertical angle in radians
    GLfloat verticalAngle;                  // vertical angle in radians

    const GLfloat maxSpeed = 200.f;         // max pan speed
    const GLfloat maxRotationSpeed = 3.0f;  // max rotation speed
    GLfloat speed;                          // pan speed
    GLfloat rotationSpeed;                  // rotation speed

    GLfloat lastTime;                       // time at last update
    GLfloat currentTime;                    // time at current update
    GLfloat deltaTime;                      // time between current and previous

    GLfloat fov;                            // field of view, in degrees

    glm::mat4 View;                         // view matrix
    glm::mat4 Projection;                   // projection matrix

    const bool *keysPressed;                // pointer to keysPressed array
    const bool *keysToggled;                // pointer to keysToggled array

public:

    Camera(bool *keysPressed,
           bool *keysToggled);              // constructor
    ~Camera();                              // destructor
    void handleInput();                     // interface with keyboard
    void update();                          // move camera
    void reset();                           // reset camera

    // getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getPosition0() const { return position0; }
    glm::mat4 getView() const { return View; }
    glm::mat4 getProjection() const { return Projection; }

    // movement functions
    void moveForward(GLfloat distance);
    void moveUp(GLfloat distance);
    void moveBackward(GLfloat distance);
    void moveDown(GLfloat distance);
    void moveRight(GLfloat distance);
    void moveLeft(GLfloat distance);
    void rotateUp(GLfloat rotationAngle);
    void rotateDown(GLfloat rotationAngle);
    void rotateRight(GLfloat rotationAngle);
    void rotateLeft(GLfloat rotationAngle);

};


#endif //PLAYGROUND_CAMERA_H
