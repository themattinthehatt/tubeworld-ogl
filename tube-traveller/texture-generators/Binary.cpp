//
// Created by mattw on 12/9/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <math.h>

#include "Binary.h"

Binary::Binary(GLuint shaderID) {

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "loadedTexture");

    // texture properties
    interpLinear = false; // false = nearest neighbor
    numTextures = 20;

    textureIDs = new GLuint[numTextures];

    numBits = 12;                    // total number of bits we'll be counting
    maxVal = (int) pow(2,numBits);  // max number, for modular arithmetic
    currNum = 0;                    // current value to represent with bits

    currIndex = 0; // index offset for textureIDs for dynamic access

    for (int i = 0; i < numTextures; ++i) {

        int width = numBits;
        int height = 1;
        GLubyte image[width][height][3];

        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < height; ++k) {
                if (Binary::bitPresent(currNum, j)) {
                    // mark a bit as present with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (0.f * 255);
                    image[j][k][2] = (GLubyte) (1.f * 255);
                } else {
                    // mark a bit as absent with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (1.f * 255);
                    image[j][k][2] = (GLubyte) (1.f * 255);
                }
            }
        }

        // use new number on next texture
        currNum = (currNum + 1) % maxVal;

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

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // Set texture filtering parameters
        if (interpLinear) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }


        // unbind the texture object
        glBindTexture(GL_TEXTURE_2D, 0);

    }

}

void Binary::update(const PathGenerator *path) {

    // update new segments
    for (int i = 0; i < path->segsToAdd; ++i) {

        // ------------------- generate new texture ----------------------------
        int width = numBits;
        int height = 1;
        GLubyte image[width][height][3];

        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < height; ++k) {
                if (Binary::bitPresent(currNum, j)) {
                    // mark a bit as present with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (0.f * 255);
                    image[j][k][2] = (GLubyte) (1.f * 255);
                } else {
                    // mark a bit as absent with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (1.f * 255);
                    image[j][k][2] = (GLubyte) (1.f * 255);
                }
            }
        }

        // use new number on next texture
        currNum = (currNum + 1) % maxVal;

        // ------------------- bind texture to existing ID ---------------------

        // generate 1 texture ID, put the resutling identifier in textureID
        glGenTextures(1, &textureIDs[currIndex]);
        glActiveTexture(GL_TEXTURE0);

        // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
        // commands will configure the currently bound texture
        glBindTexture(GL_TEXTURE_2D, textureIDs[currIndex]);

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

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // Set texture filtering parameters
        if (interpLinear) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        // unbind the texture object
        glBindTexture(GL_TEXTURE_2D, 0);


        // ------------------- update index into textureIDs --------------------
        currIndex = (currIndex + 1) % numTextures;

    }

};

void Binary::draw(int index) {

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0 + index);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, index);

}

void Binary::clean() {
    delete[] textureIDs;
}

bool Binary::bitPresent(int val, int index) {
    return ((val >> index) & 1) == 1;
}