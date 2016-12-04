//
// Created by mattw on 12/3/16.
//

#ifndef TUBEWORLD_TEXTURECYLINDER_H
#define TUBEWORLD_TEXTURECYLINDER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../core/Global.h"
#include "../../core/Camera.h"
#include "../path-generators/PathGenerator.h"

class TextureCylinder : public TubeGenerator {
private:
public:

    std::vector<glm::vec3> cylinderModelCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    GLint numCenters;
    GLint numVerticesPerInstance;

    glm::vec3 *g_center_buffer_data;
    glm::mat4 *g_rotation_buffer_data;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint centerBufferID;
    GLuint uvBufferID;
    GLuint rotationBufferID;
    GLuint shaderID;
    GLuint textureID;
    GLuint samplerID;
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
    TextureCylinder(GLint numCenters);
    // update positions/angles of tube elements
    void update(const PathGenerator *path, Camera &cam);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_TEXTURECYLINDER_H