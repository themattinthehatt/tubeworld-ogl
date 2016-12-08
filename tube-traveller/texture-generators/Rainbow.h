//
// Created by mattw on 12/8/16.
//

#ifndef TUBEWORLD_RAINBOW_H
#define TUBEWORLD_RAINBOW_H

#include <GL/glew.h>
#include "TextureGenerator.h"

class Rainbow : public TextureGenerator {
private:
public:

    // constructor
    Rainbow(GLuint shaderID);
    // update texture samplers
    void update();
    // bind textures
    void draw(int index);
    // clean up VAOs, VBOs, etc.
    void clean();
    // hsv to rgb function
    void hsvToRgb(float h, float s, float v,
                  float* r, float* g, float* b);

};

#endif //TUBEWORLD_RAINBOW_H
