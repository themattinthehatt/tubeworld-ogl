//
// Created by mattw on 1/14/17.
//

#ifndef TUBEWORLD_PERLINBLOCKPORTAL_H
#define TUBEWORLD_PERLINBLOCKPORTAL_H

#include <glm/glm.hpp>
#include "../Portal.h"
#include "../../core/Shader.h"
#include "../../core/Player.h"
#include "../../core/Camera.h"

class PerlinBlockPortal : public Portal {
private:
public:

    // portal properties II
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    GLint numVertices;

    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;

    // texture properties
    GLint samplerID;
    GLuint textureID;

    Shader *shader;


    // constructor
    PerlinBlockPortal(glm::vec3 location, glm::vec3 heading, const char* file_loc);
    // update portal
    void update(Camera cam, Player player);
    // draw portal
    void draw();
    // clean portal
    void clean();

};


#endif //TUBEWORLD_PERLINBLOCKPORTAL_H
