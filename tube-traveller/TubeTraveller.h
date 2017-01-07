//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.


#ifndef TUBEWORLD_TUBETRAVELLER_H
#define TUBEWORLD_TUBETRAVELLER_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "path-generators/PathGenerator.h"
#include "tube-generators/TubeGenerator.h"
#include "tube-generators/TextureCylinderLight.h"
#include "../core/Skybox.h"

class TubeTraveller {
private:


public:

    Skybox *skybox;
    PathGenerator *path;
    TubeGenerator *tube;

    IOHandler &io;

    enum PathGeneratorType {
        PATH_CIRCLE,
        PATH_USER,
        PATH_RANDOM,
        MAX_NUM_PATHS
    };
    enum TubeType {
        TUBE_CUBES_SQ,
        TUBE_CUBES_CIRC,
        TUBE_CYLINDER,
        TUBE_TEXTURE_CYLINDER_LIGHT,
        MAX_NUM_TUBES
    };

    enum PlayerMode {
        PLAYER_BOUND,
        PLAYER_FREE,
        MAX_PLAYER_MODES
    };
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;


    // constructor
    TubeTraveller(GLint numCenters,
                  PathGeneratorType pathType,
                  TubeType tubeType,
                  TextureCylinderLight::TextureType textureType,
                  TextureCylinderLight::LightStyle lightStyle);
    // update dynamics of cube array
    void update(Camera &cam, Player &player);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_TUBETRAVELLER_H
