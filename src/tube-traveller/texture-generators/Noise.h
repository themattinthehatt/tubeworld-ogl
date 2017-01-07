//
// Created by mattw on 12/17/16.
//

#ifndef TUBEWORLD_NOISE_H
#define TUBEWORLD_NOISE_H

#include <math.h>
#include <GL/glew.h>
#include "TextureGenerator.h"
#include "../path-generators/PathGenerator.h"
#include "../../../lib/PerlinNoise.h"

class Noise : public TextureGenerator {
private:
public:

    // for indexing textureIDs array
    GLint currIndex;

    // for moving through perlin noise space
    unsigned int seed;
    PerlinNoise pn;
    double pnVal;

    // texture properties
    int width;
    int height;
    bool interpLinear;
    bool woodGrain;
    bool mirrorTex;

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
