//
// Created by mattw on 8/21/16.
//

#ifndef TUBEWORLD_CUBETUBE_H
#define TUBEWORLD_CUBETUBE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "PathUserInput.h"

class CubeTube {
private:
public:

    const GLfloat PI = 3.14159f;

    GLuint numVertices = 36;
    static glm::vec3 cubeModelCoordinates[];

    int numCubesHorizontal;
    int numCubesVertical;
    int numCenters;
    int numVerticesPerInstance;
    GLuint numModelsPerRing;

    glm::vec3 *ringOffsets;
    glm::vec3 *g_center_buffer_data;
    glm::vec3 *modelOffsets;
    glm::vec3 *g_radial_buffer_data;
    glm::mat4 *rotationMatrix;
    glm::mat4 *g_rotation_buffer_data;

    GLfloat sideLength;
    GLfloat spacing;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint centerBufferID;
    GLuint radialBufferID;
    GLuint rotationBufferID;
    GLuint shaderID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLuint mMatrixID;
    GLuint vpMatrixID;
    GLuint mvpMatrixID;
    GLfloat time;
    GLuint timeParamID;

    // constructor
    CubeTube(GLuint numCubesHorizontal, GLuint numCubesVertical, GLuint numCenters);
    // update positions/angles of tube elements
    void update(const PathUserInput &path, const Camera &cam);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_CUBETUBE_H
