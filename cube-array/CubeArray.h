//
// Created by mattw on 8/16/16.
//

#ifndef TUBEWORLD_CUBEARRAY_H
#define TUBEWORLD_CUBEARRAY_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Camera.h"

class CubeArray {
private:


public:
    GLuint numVertices = 36;
    static GLfloat cubeModelCoordinates[];
    static GLfloat cubeColorCoordinates[];
    static GLfloat cubeSTCoordinates[];

    bool isTextureRendered;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint texture;
    GLuint textureID;
    GLuint shaderID;
    GLuint colorBufferID;
    GLuint stBufferID;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 mvpMatrix;
    GLuint mvpMatrixID;
    GLfloat time;
    GLuint timeParamID;

    int numCubesX;
    int numCubesY;
    int numCubesZ;

    // constructor
    CubeArray(int numCubesX, int numCubesY, int numCubesZ,
              bool isTextureRendered);
    // destructor
    ~CubeArray();
    // update dynamics of cube array
    void update(const Camera &cam);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

    // create initial position data for each vertex
    static void createCubeArray(int numCubesX, int numCubesY, int numCubesZ,
                                int numVertices, GLfloat *buffer_data);
    // create initial color data for each vertex
    static void createColorArray(int numCubes, int numVertices,
                                 GLfloat *buffer_data);
    // create initial texture coordinate data for each vertex
    static void createSTArray(int numCubes, int numVertices,
                              GLfloat *buffer_data);

};


#endif //TUBEWORLD_CUBEARRAY_H
