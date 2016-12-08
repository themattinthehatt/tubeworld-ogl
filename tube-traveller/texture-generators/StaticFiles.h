//
// Created by mattw on 12/7/16.
//

#ifndef TUBEWORLD_STATICFILES_H
#define TUBEWORLD_STATICFILES_H

#include <GL/glew.h>
#include "TextureGenerator.h"

class StaticFiles : public TextureGenerator {
private:
public:

    // constructor
    StaticFiles(GLuint shaderID);
    // update texture samplers
    void update();
    // bind textures
    void draw(int index);
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_STATICFILES_H
