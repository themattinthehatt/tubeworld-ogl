//
// Created by mattw on 12/17/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <math.h>

#include "Noise.h"

Noise::Noise(GLuint shaderID) {

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "loadedTexture");

    // texture properties
    numTextures = 20;
    numLevels = 4; // number of noise levels
    interpLinear = false;

    textureIDs = new GLuint[numTextures];
    currIndex = 0; // index offset for textureIDs for dynamic access

    for (int i = 0; i < numTextures; ++i) {

        int width = (int) pow(2, numLevels);
        int height = (int) pow(2, numLevels);
        GLubyte image[height][width][4];

        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < width; ++k) {
                image[j][k][0] = (GLubyte) (1.f * 255);
                image[j][k][1] = (GLubyte) (1.f * 255);
                image[j][k][2] = (GLubyte) (1.f * 255);
                image[j][k][3] = (GLubyte) 255;
            }
        }

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

void Noise::update(const PathGenerator *path) {

    // update new segments
    for (int i = 0; i < path->segsToAdd; ++i) {

        // ------------------- generate new texture ----------------------------
        int width = (int) pow(2, numLevels);
        int height = (int) pow(2, numLevels);
        GLubyte image[height][width][4];

        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < width; ++k) {
                // mark a bit as absent with following color
                image[j][k][0] = (GLubyte) (1.f * 255);
                image[j][k][1] = (GLubyte) (1.f * 255);
                image[j][k][2] = (GLubyte) (1.f * 255);
                image[j][k][3] = (GLubyte) 255;
            }
        }

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

void Noise::draw(int index) {

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0 + index);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, index);

}

void Noise::clean() {
    delete[] textureIDs;
}

// adapted from http://stackoverflow.com/a/8208967
void Noise::hsvToRgb(float h, float s, float v,
                       float* r, float* g, float *b) {

    int i = static_cast<int>(floor(h * 6));
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: *r = v, *g = t, *b = p; break;
        case 1: *r = q, *g = v, *b = p; break;
        case 2: *r = p, *g = v, *b = t; break;
        case 3: *r = p, *g = q, *b = v; break;
        case 4: *r = t, *g = p, *b = v; break;
        case 5: *r = v, *g = p, *b = q; break;
    }

}