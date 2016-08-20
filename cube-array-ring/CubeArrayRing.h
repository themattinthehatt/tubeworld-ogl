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

class CubeArrayRing {
private:


public:
    GLuint numVertices = 36;
    static glm::vec3 cubeModelCoordinates[];

    GLuint numModelsPerRing;
    glm::vec3 *modelOffsets;
    glm::vec3 *ringOffsets;
    glm::vec3 normalVec;

    GLfloat sideLength;
    GLfloat spacing;


    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint shaderID;
    GLuint centerBufferID;
    GLuint radialBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLuint mMatrixID;
    GLuint vpMatrixID;
    GLuint mvpMatrixID;
    glm::vec3 playerPos;
    glm::vec3 playerPos0;
    glm::vec3 tubeOrigin;
    GLfloat time;
    GLuint timeParamID;

    int numCubesHorizontal;
    int numCubesVertical;
    int numCubeRings;
    int numVerticesPerInstance;
    int numCubes;

    // constructor
    CubeArrayRing(int numCubesHorizontal, int numCubesVertical, int numCubeRings);
    // destructor
    ~CubeArrayRing();
    // update dynamics of cube array
    void update(const Camera &cam);
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
