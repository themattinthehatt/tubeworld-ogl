//
// Created by mattw on 12/7/16.
//

#ifndef TUBEWORLD_TEXTUREGENERATOR_H
#define TUBEWORLD_TEXTUREGENERATOR_H

#include "../path-generators/PathGenerator.h"

class TextureGenerator {
private:
public:

    GLint numTextures;
    GLuint *textureIDs;
    GLint samplerID;

    // update texture samplers
    virtual void update(const PathGenerator *path) = 0;
    // bind textures
    virtual void draw(int index) = 0;
    // clean up VAOs, VBOs, etc.
    virtual void clean() = 0;

};

#endif //TUBEWORLD_TEXTUREGENERATOR_H
