//
// Created by mattw on 8/14/16.
//

#ifndef PLAYGROUND_SKYBOX_H
#define PLAYGROUND_SKYBOX_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Camera.h"

class Skybox {

private:
    GLfloat multiplier;         // half-width of skybox
    GLuint numVertices = 36;    // number of vertices in a cube
    static GLfloat skyboxModelCoordinates[];

    enum RenderMode {
        RENDER_CUBEMAP0,
        RENDER_BLACK,
        RENDER_WHITE,
        MAX_RENDER_MODES
    };

public:
    GLuint vertexArrayID;       // ID of VAO
    GLuint vertexBufferID;      // ID of VBO
    GLuint texture;             // ID of texture (for binding)
    GLuint textureID;           // ID of texture (for cube sampler)
    GLuint shaderID;            // ID of shader program
    glm::mat4 viewMatrix;       // view matrix
    GLuint viewMatrixID;        // ID of view matrix for shaders

    const bool *keysPressed;    // pointer to keysPressed array
    const bool *keysToggled;    // pointer to keysToggled array
    bool renderTrigger = false; // returns true upon release of proper key
    GLuint renderMode = 0;      // specifies current mode of skybox rendering

    // constructor
    Skybox(std::vector<const GLchar*> files, GLfloat multiplier,
           const bool *keysPressed, const bool *keysToggled);
    // destructor
    ~Skybox();
    // update skybox position
    void update(const Camera &cam);
    // draw skybox
    void draw();
    // clear up VAOs, VBOs, etc.
    void clean();

    // external function to load BMPs as a cubemap
    static GLuint loadCubemap(std::vector<const GLchar*> files);
};


#endif //PLAYGROUND_SKYBOX_H