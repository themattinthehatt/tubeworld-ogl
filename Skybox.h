//
// Created by mattw on 8/14/16.
//

#ifndef PLAYGROUND_SKYBOX_H
#define PLAYGROUND_SKYBOX_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Camera.h"

class Skybox {

private:
    GLfloat multiplier;
    GLuint numVertices = 36;
    static GLfloat skyboxModelCoordinates[];

public:
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint texture;
    GLuint textureID;
    GLuint shaderID;
    glm::mat4 viewMatrix;
    GLuint viewMatrixID;

    Skybox(std::vector<const GLchar*> files_, GLfloat multiplier_);
    void update(const Camera &cam);
    void draw();
    void clean();
    ~Skybox();

    static GLuint loadCubemap(std::vector<const GLchar*> files);
};


#endif //PLAYGROUND_SKYBOX_H