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
    double pnScale;

    // texture properties
    struct TexProperties {
        int width = 128;            // width of noise texture
        int height = 128;           // height of noise texture
        bool interpLinear = true;   // linearly interpolate texture (false=nearest)
        bool woodGrain = false;     // render "woodGrain" style instead
        bool mirrorTex = true;      // mirror texture around tube centerline
        bool grayscale = false;     // render grayscale noise
        GLfloat threshVal = 0.f;    // all values below threshVal are not rendered
    };
    TexProperties texProps;

    // constructor
    Noise(GLuint shaderID, TexProperties texProps);
    // update texture samplers
    void update(const PathGenerator *path);
    // bind textures
    void draw(int index);
    // clean up VAOs, VBOs, etc.
    void clean();
    // generate noise image
    void generateImage(int index);
    // hsv to rgb function
    static void hsvToRgb(float h, float s, float v,
                  float* r, float* g, float* b);

};

#endif //TUBEWORLD_NOISE_H
