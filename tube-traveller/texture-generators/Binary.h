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

    int numBits;
    GLuint currNum;
    int maxVal;

    // for indexing textureIDs array
    GLint currIndex;
//    GLint

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
