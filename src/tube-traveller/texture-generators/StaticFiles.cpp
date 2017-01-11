//
// Created by mattw on 12/7/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "StaticFiles.h"
#include "../../core/loaders/loadTextures.h"

StaticFiles::StaticFiles(GLuint shaderID) {

    // location of textures
    std::vector<const char*> file_loc = {"data/textures/temp2.bmp",
                                         "data/textures/temp3.bmp"};

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "material.loadedTexture");

    numTextures = static_cast<GLint>(file_loc.size());

    textureIDs = new GLuint[numTextures];

    for (int i = 0; i < numTextures; ++i) {

        int width, height;
        unsigned char *image = loadBMP(file_loc[i], &width, &height);

        // generate 1 texture ID, put the resutling identifier in textureID
        glGenTextures(1, &textureIDs[i]);
        glActiveTexture(GL_TEXTURE0);

        // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
        // commands will configure the currently bound texture
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // generate the texture by using the previously loaded image data
        glTexImage2D(
                GL_TEXTURE_2D,     // texture target; will gen texture on textureIDs[i]
                0,                 // mipmap level; use base of 0
                GL_RGB,            // type of format we want to store the texture
                width,
                height,
                0,                 // legacy bs
                GL_RGB,            // format of source image
                GL_UNSIGNED_BYTE,  // format of source image
                image              // source image
        );
        // textureIDs[i] now has texture image attached to it
        // glGenerateMipmap here if desired

        // free image memory
        delete[] image;

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // unbind the texture object
        glBindTexture(GL_TEXTURE_2D, 0);

    }

}

void StaticFiles::update(const PathGenerator *path){};

void StaticFiles::draw(int index) {

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0); // GL_TEXTURE0+index to render both textures
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, index);

}

void StaticFiles::clean() {
    glDeleteTextures(numTextures, textureIDs);
    delete[] textureIDs;
}