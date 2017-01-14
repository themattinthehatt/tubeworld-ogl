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
#include "../core/FramebufferObject.h"

class TubeTraveller {
private:


public:

    // for drawing tube
    Skybox *skybox;
    PathGenerator *path;
    TubeGenerator *tube;

    // for post-processing
    FramebufferObject *fbo;
    Shader *postShader;
    GLfloat fadeStep;
    GLfloat fadeTotal;
    glm::vec3 fadeColor;
    GLint fadeStepID;
    GLint fadeTotalID;
    GLint fadeColorID;

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
    // update dynamics of tube
    void update(Camera &cam, Player &player);
    // draw tube
    void draw();
    // draw tube to framebuffer
    void renderOffscreen();
    // play with post-processing effects
    void postProcess();
    // clean up VAOs, VBOs, etc.
    void clean();

    // setters
    void setFadeStep(GLfloat fadeStep_) {fadeStep = fadeStep_; };
    void setFadeTotal(GLfloat fadeTotal_) {fadeTotal = fadeTotal_; };

};


#endif //TUBEWORLD_TUBETRAVELLER_H
