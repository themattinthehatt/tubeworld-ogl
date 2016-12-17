//
// Created by mattw on 12/9/16.
//

#ifndef TUBEWORLD_BINARY_H
#define TUBEWORLD_BINARY_H

#include <math.h>
#include <GL/glew.h>
#include <bitset>
#include "TextureGenerator.h"
#include "../path-generators/PathGenerator.h"

class Binary : public TextureGenerator {
private:
public:

    int numBits;    // number of bits for binary number texture
    GLuint currNum; // current number in the counting
    int maxVal;     // max value; 2^numBits; width of texture in bits
    int textHeight; // height of texture in bits

    // for indexing textureIDs array
    GLint currIndex;

    // texture properties
    bool interpLinear; // 1 for yes, 0 for nearest neighbor


    // constructor
    Binary(GLuint shaderID);
    // update texture samplers
    void update(const PathGenerator *path);
    // bind textures
    void draw(int index);
    // clean up VAOs, VBOs, etc.
    void clean();
    // boolean function to determine location of bits
    static bool bitPresent(int val, int index);

};

#endif //TUBEWORLD_BINARY_H
