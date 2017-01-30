//
// Created by mattw on 12/9/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <math.h>

#include "Binary.h"

Binary::Binary(GLuint shaderID, TexProperties texProps_) {

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "material.loadedTexture");

    // texture properties
    numTextures = 20;
    texProps = texProps_;
    maxVal = (int) pow(2,texProps.numBits); // max number, for mod arithmetic
    currNum = 0;                            // current val to rep with bits

    textureIDs = new GLuint[numTextures];
    currIndex = 0; // index offset for textureIDs for dynamic access

    for (int i = 0; i < numTextures; ++i) {
        generateImage(i);
    }

}

void Binary::update(const PathGenerator *path) {

    // update new segments
    for (int i = 0; i < path->segsToAdd; ++i) {

        // generate new texture
        generateImage(currIndex);

        // update index into textureIDs
        currIndex = (currIndex + 1) % numTextures;

    }

}

void Binary::draw(int index) {

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, 0);

}

void Binary::clean() {
    glDeleteTextures(numTextures, textureIDs);
    delete[] textureIDs;
}

void Binary::generateImage(int index) {

    int width = texProps.numBits;
    int height = texProps.height;
    GLubyte image[height][width][4];

    for (int j = 0; j < height; ++j) {
        for (int k = 0; k < width; ++k) {
            if (texProps.grayscale) {
                if (Binary::bitPresent(currNum, k)) {
                    // mark a bit as present with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (1.f * 255);
                    image[j][k][2] = (GLubyte) (1.f * 255);
                    image[j][k][3] = (GLubyte) 255;
                } else {
                    // mark a bit as absent with following color
                    image[j][k][0] = (GLubyte) (0.f * 255);
                    image[j][k][1] = (GLubyte) (0.f * 255);
                    image[j][k][2] = (GLubyte) (0.f * 255);
                    image[j][k][3] = (GLubyte) 255;
                }
            } else {
                if (Binary::bitPresent(currNum, k)) {
                    // mark a bit as present with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (0.f * 255);
                    image[j][k][2] = (GLubyte) (0.f * 255);
                    image[j][k][3] = (GLubyte) 255;
                } else {
                    // mark a bit as absent with following color
                    image[j][k][0] = (GLubyte) (1.f * 255);
                    image[j][k][1] = (GLubyte) (1.f * 255);
                    image[j][k][2] = (GLubyte) (1.f * 255);
                    image[j][k][3] = (GLubyte) 255;
                }
            }
        }
    }

    // use new number on next texture
    currNum = (currNum + 1) % maxVal;

    // generate 1 texture ID, put the resutling identifier in textureID
    glGenTextures(1, &textureIDs[index]);
    glActiveTexture(GL_TEXTURE0);

    // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
    // commands will configure the currently bound texture
    glBindTexture(GL_TEXTURE_2D, textureIDs[index]);

    // generate the texture by using the previously loaded image data
    glTexImage2D(
            GL_TEXTURE_2D,     // texture target; will gen texture on textureIDs[i]
            0,                 // mipmap level; use base of 0
            GL_RGBA,           // type of format we want to store the texture
            width,
            height,
            0,                 // legacy bs
            GL_RGBA,           // format of source image
            GL_UNSIGNED_BYTE,  // format of source image
            image              // source image
    );
    // textureIDs[i] now has texture image attached to it
    // glGenerateMipmap here if desired

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Set texture filtering parameters
    if (texProps.interpLinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // unbind the texture object
    glBindTexture(GL_TEXTURE_2D, 0);

}

bool Binary::bitPresent(int val, int index) {
    return ((val >> index) & 1) == 1;
}