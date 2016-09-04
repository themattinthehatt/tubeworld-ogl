//
// Created by mattw on 9/4/16.
//

#ifndef TUBEWORLD_PHYSICSPHERES_H
#define TUBEWORLD_PHYSICSPHERES_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../core/Global.h"
#include "../core/Camera.h"

class PhysicSpheres {
private:
public:

    std::vector<glm::vec3> sphereModelCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLint numVertices;

    GLint numCenters;
    GLint numVerticesPerInstance;

    glm::vec3 *ringCenters;
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

    IOHandler &io;

    // constructor
    PhysicSpheres();
    // update positions/angles of tube elements
    void update(Camera &cam, Player &player);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_PHYSICSPHERES_H
