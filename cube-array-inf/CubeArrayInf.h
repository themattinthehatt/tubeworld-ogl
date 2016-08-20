//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

#ifndef TUBEWORLD_CUBEARRAYINF_H
#define TUBEWORLD_CUBEARRAYINF_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Camera.h"

class CubeArrayInf {
private:


public:
    GLuint numVertices = 36;
    static GLfloat cubeModelCoordinates[];
    static GLfloat cubeSTCoordinates[];
    glm::vec3 *g_center_buffer_data;

    bool isTextureRendered;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint texture;
    GLuint textureID;
    GLuint shaderID;
    GLuint centerBufferID;
    GLuint colorBufferID;
    GLuint stBufferID;
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

    int numCubesX;
    int numCubesY;
    int numCubesZ;
    int numCubes;

    // constructor
    CubeArrayInf(int numCubesX, int numCubesY, int numCubesZ,
              bool isTextureRendered);
    // destructor
    ~CubeArrayInf();
    // update dynamics of cube array
    void update(const Camera &cam);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

    // create initial position data for each vertex
    static void createCubeArray(int numCubesX, int numCubesY, int numCubesZ,
                                glm::vec3 center_buffer_data[]);
    // create initial texture coordinate data for each vertex
    static void createSTArray(int numCubes, int numVertices,
                              GLfloat *buffer_data);
    // create initial position data for each vertex
    static void createHollowCubeArray(int numCubesX, int numCubesY, int numCubesZ,
                                         glm::vec3 origin,
                                         glm::vec3 center_buffer_data[]);

};


#endif //TUBEWORLD_CUBEARRAYINF_H
