//
// Created by mattw on 8/14/16.
//
#include <vector>
#include <GLFW/glfw3.h>
#include "loadTextures.h"

GLuint loadCubemap(std::vector<const char*> faces) {

    // create 1 OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    // "Bind" the newly created texture: all future texture functions will
    // modify this texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height;
    for (GLuint i = 0; i < faces.size(); ++i) {
        unsigned char *image = loadBMP(faces[i], &width, &height);
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image
        );
        delete[] image;
    }

    // specify the wrapping and filtering methods for the texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // unbind the texture object
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}