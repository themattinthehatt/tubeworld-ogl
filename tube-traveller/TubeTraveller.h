//
// Created by mattw on 8/16/16.
//
// use the standard vertex attributes for data per vertex and use the instanced
// array for storing data that is unique per instance.

// TODO
// (DONE) single image repeated init example
//     variable number of textures
//          split numCenters into the number of textures (which may be periodic
//              or only a single texture per center
//          render each set of tubes that have the same texture at a time
//          TextureGenerator class should also generate an array of indices for
//              each tube that has the same texture (loop over number of elements
//              in this index array, need to send center_buffer_data and model
//              rotations
// TextureGenerator class
//      double texture
//      ROYGBIV
//      1/f noise?
// figure out how to incorporate lighting into the TubeGenerator class (perhaps
//      leave out of TubeSimpleShape for simplicity - can always go back later)

#ifndef TUBEWORLD_TUBETRAVELLER_H
#define TUBEWORLD_TUBETRAVELLER_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "path-generators/PathGenerator.h"
#include "tube-generators/TubeGenerator.h"
#include "texture-generators/TextureGenerator.h"

class TubeTraveller {
private:


public:

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
        TUBE_TEXTURE_CYLINDER,
        MAX_NUM_TUBES
    };
    enum TextureType {
        TEXTURE_FILES_STATIC,
        TEXTURE_RAINBOW,
        MAX_NUM_TEXTURES
    };

    enum PlayerMode {
        PLAYER_BOUND,
        PLAYER_FREE,
        MAX_PLAYER_MODES
    };
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;


    // constructor
    TubeTraveller(GLint numCenters);
    // update dynamics of cube array
    void update(Camera &cam, Player &player);
    // draw cube array
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_TUBETRAVELLER_H
