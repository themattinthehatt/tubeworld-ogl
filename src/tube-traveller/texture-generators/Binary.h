//
// Created by mattw on 12/9/16.
//

#ifndef TUBEWORLD_BINARY_H
#define TUBEWORLD_BINARY_H

#include <math.h>
#include <GL/glew.h>
#include "TextureGenerator.h"
#include "../path-generators/PathGenerator.h"

class Binary : public TextureGenerator {
private:
public:

    // keep track of counting
    GLuint currNum; // current number in the counting
    int maxVal;     // max value; 2^numBits; width of texture in bits

    // for indexing textureIDs array
    GLint currIndex;

    // texture properties
    struct TexProperties {
        int numBits = 12;           // width of texture
        int height = 3;             // height of texture
        bool interpLinear = false;  // 1 for yes, 0 for nearest neighbor
        bool grayscale = false;     // render image in grayscale
    };
    TexProperties texProps;

    // constructor
    Binary(GLuint shaderID, TexProperties texProps);
    // update texture samplers
    void update(const PathGenerator *path);
    // bind textures
    void draw(int index);
    // clean up VAOs, VBOs, etc.
    void clean();
    // generate binary image
    void generateImage(int index);
    // boolean function to determine location of bits
    static bool bitPresent(int val, int index);

};

#endif //TUBEWORLD_BINARY_H
