//
// Created by mattw on 12/17/16.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <functional>

#include "Noise.h"

Noise::Noise(GLuint shaderID, TexProperties texProps_) {

    // get an ID for our texture uniform
    samplerID = glGetUniformLocation(shaderID, "material.loadedTexture");

    // texture properties
    numTextures = 100;
    texProps = texProps_;

    // set up perlin noise
    seed = 236;
    pn = PerlinNoise(seed);
    pnVal = 0.0;

    textureIDs = new GLuint[numTextures];
    currIndex = 0; // index offset for textureIDs for dynamic access

    for (int i = 0; i < numTextures; ++i) {
        generateImage(i);
    }

}

void Noise::update(const PathGenerator *path) {

    // update new segments
    for (int i = 0; i < path->segsToAdd; ++i) {

        // generate new texture
        generateImage(currIndex);

        // update index into textureIDs
        currIndex = (currIndex + 1) % numTextures;

    }

};

void Noise::draw(int index) {

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
    // puts the texture in texture unit 0
    glUniform1i(samplerID, 0);

}

void Noise::clean() {
    glDeleteTextures(numTextures, textureIDs);
    delete[] textureIDs;
}

void Noise::generateImage(int index) {


    GLubyte image[texProps.height][texProps.width][4];

    float hue = 1;
    float sat = 1;
    float val = 1;
    float r, g, b;
    GLubyte red, green, blue;
    GLubyte alpha = 255;

    int width;
    if (texProps.mirrorTex) {
        width = texProps.width / 2;
    } else {
        width = texProps.width;
    }

    double y;
    for (int j = 0; j < texProps.height; ++j) {
        pnVal += 0.2f * (1.f / texProps.height);
        for (int k = 0; k < width; ++k) {

            // get current image coordinates
            y = 5 * ((double) k + 1) / ((double) texProps.width);

            // get corresponding perlin noise value
            hue = static_cast<float>(pn.noise(pnVal, y, 0.8));
            if (texProps.woodGrain) {
                // first val must be >= second val
                // first val controls how much detail there is
                hue = 10.f * hue - floor(10.f * hue); // wood grain
            }

            // get rgb values
            if (texProps.grayscale) {
                float linterp = 0.0;
                if (hue < texProps.threshVal - linterp) {
                    hue = 0.f;
                } else if (hue < texProps.threshVal) {
                    hue = (hue - (texProps.threshVal - linterp)) / linterp;
                }
                red = (GLubyte) floor(255 * hue);
                green = (GLubyte) floor(255 * hue);
                blue = (GLubyte) floor(255 * hue);
            } else {
                // translate noise value into a hue value
                hsvToRgb(hue, sat, val, &r, &g, &b);
                red = (GLubyte) floor(255 * r);
                green = (GLubyte) floor(255 * g);
                blue = (GLubyte) floor(255 * b);
            }

            // fill image
            image[j][k][0] = red;
            image[j][k][1] = green;
            image[j][k][2] = blue;
            image[j][k][3] = alpha;
            if (texProps.mirrorTex) {
                image[j][texProps.width-k-1][0] = red;
                image[j][texProps.width-k-1][1] = green;
                image[j][texProps.width-k-1][2] = blue;
                image[j][texProps.width-k-1][3] = alpha;
            }

        }
    }

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
            texProps.width,
            texProps.height,
            0,                 // legacy bs
            GL_RGBA,           // format of source image
            GL_UNSIGNED_BYTE,  // format of source image
            image              // source image
    );
    // textureIDs[i] now has texture image attached to it
    // glGenerateMipmap here if desired

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
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