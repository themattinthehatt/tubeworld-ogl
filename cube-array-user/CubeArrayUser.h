//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

#ifndef TUBEWORLD_CUBEARRAYUSER_H
#define TUBEWORLD_CUBEARRAYUSER_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Camera.h"

class CubeArrayUser {
private:


public:

    enum PlayerMode {
        PLAYER_FREE,
        PLAYER_BOUND,
        MAX_PLAYER_MODES
    };

    GLuint numVertices = 36;
    static glm::vec3 cubeModelCoordinates[];

    GLuint numModelsPerRing;

    glm::vec3 *ringOffsets;
    glm::vec3 *g_center_buffer_data;
    glm::vec3 *modelOffsets;
    glm::vec3 *g_radial_buffer_data;
    glm::vec3 *normalVec;
    glm::mat4 *rotationMatrix;
    glm::mat4 *g_rotation_buffer_data;

    GLfloat *horizontalAngles;
    GLfloat *verticalAngles;
    glm::vec3 *positions;
    glm::vec3 *headings;
    glm::vec3 *ups;
    glm::vec3 *rights;
    GLfloat speed = 10.f;
    GLfloat deltaHorizontalAngle = 0.0f;
    GLfloat deltaVerticalAngle = 0.0f;
    GLfloat decayRightHorizontalAngle = 0.0f;
    GLfloat decayLeftHorizontalAngle = 0.0f;
    GLfloat decayUpVerticalAngle = 0.0f;
    GLfloat decayDownVerticalAngle = 0.0f;
    GLfloat growthRightHorizontalAngle = 0.0f;
    GLfloat growthLeftHorizontalAngle = 0.0f;
    GLfloat growthUpVerticalAngle = 0.0f;
    GLfloat growthDownVerticalAngle = 0.0f;

    GLfloat sideLength;
    GLfloat spacing;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint shaderID;
    GLuint centerBufferID;
    GLuint radialBufferID;
    GLuint rotationBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLuint mMatrixID;
    GLuint vpMatrixID;
    GLuint mvpMatrixID;
    GLfloat time;
    GLuint timeParamID;

    int numCubesHorizontal;
    int numCubesVertical;
    int numCubeRings;
    int numVerticesPerInstance;

    const bool *keysPressed;    // pointer to keysPressed array
    const bool *keysToggled;    // pointer to keysToggled array
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;


    // constructor
    CubeArrayUser(int numCubesHorizontal, int numCubesVertical, int numCubeRings,
                    const bool *keysPressed, const bool *keysToggled);
    // destructor
    ~CubeArrayUser();
    // update dynamics of cube array
    void update(Camera &cam, Player &player);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

    // create initial position data for each vertex
    static void createCubeTube(int numCubesX, int numCubesY, int numCubesZ,
                                         glm::vec3 origin, float cubeSpacing,
                                         glm::vec3 center_buffer_data[]);

};


#endif //TUBEWORLD_CUBEARRAYRING_H
