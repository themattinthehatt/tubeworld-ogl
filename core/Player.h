//
// Created by mattw on 8/20/16.
//

#ifndef TUBEWORLD_PLAYER_H
#define TUBEWORLD_PLAYER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const float PI = 3.141592653589f;

class Player
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

    const bool *keysPressed;                // pointer to keysPressed array
    const bool *keysToggled;                // pointer to keysToggled array

public:

    Player(bool *keysPressed,
           bool *keysToggled);              // constructor
    ~Player();                              // destructor
    void handleInput();                     // interface with keyboard
    void update();                          // move camera
    void reset();                           // reset camera

    // getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getPosition0() const { return position0; }
    glm::vec3 getHeading() const { return heading; }
    glm::vec3 getRight() const { return right; }
    glm::vec3 getUp() const { return up; }

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


#endif //TUBEWORLD_PLAYER_H
