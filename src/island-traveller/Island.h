//
// Created by mattw on 1/7/17.
//

#ifndef TUBEWORLD_ISLAND_H
#define TUBEWORLD_ISLAND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Skybox.h"
#include "../core/Shader.h"

class Island {
private:


public:

    std::vector<glm::vec3> islandCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLint numVertices;

    Shader *shader;
//    Light *light;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLfloat time;
    GLint timeParamID;

    IOHandler &io;

    // constructor
    Island();
    // update dynamics of island
    void update(Camera &cam, Player &player);
    // draw island
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_ISLAND_H
