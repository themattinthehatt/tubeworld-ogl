//
// Created by mattw on 12/17/16.
//

#ifndef TUBEWORLD_NOISE_H
#define TUBEWORLD_NOISE_H

#include <math.h>
#include <GL/glew.h>
#include "TextureGenerator.h"
#include "../path-generators/PathGenerator.h"

class Noise : public TextureGenerator {
private:
public:

    int numLevels;  // number of noise levels

    // for indexing textureIDs array
    GLint currIndex;

    bool interpLinear;

    // constructor
    Noise(GLuint shaderID);
    // update texture samplers
    void update(const PathGenerator *path);
    // bind textures
    void draw(int index);
    // clean up VAOs, VBOs, etc.
    void clean();
    // hsv to rgb function
    void hsvToRgb(float h, float s, float v,
                  float* r, float* g, float* b);

};

#endif //TUBEWORLD_NOISE_H
