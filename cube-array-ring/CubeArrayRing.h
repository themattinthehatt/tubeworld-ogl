//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

#ifndef TUBEWORLD_CUBEARRAYRING_H
#define TUBEWORLD_CUBEARRAYRING_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Camera.h"
#include "../core/IOHandler.h"

class CubeArrayRing {
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

    IOHandler &io;
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;


    // constructor
    CubeArrayRing(int numCubesHorizontal, int numCubesVertical, int numCubeRings);
    // destructor
    ~CubeArrayRing();
    // update dynamics of cube array
    void update(Camera &cam, Player &player);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_CUBEARRAYRING_H
