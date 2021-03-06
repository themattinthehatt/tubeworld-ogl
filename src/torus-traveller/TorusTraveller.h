//
// Created by mattw on 2/5/17.
//

/* TODO
 * - perlin lamp in black space
 *      - (DONE) perlin noise mapping
 *      - (DONE) HDR lighting
 *      - bloom
 * - make lamp rotate around in a torus
 *      - how to close/render the torus?
 *      - 2 level perlin noise mapping on inside of tube (optional)
 * - lamp in torus in torus
 * - torus transition (4 levels; in lowest level most of the time)
 */

#ifndef TUBEWORLD_TORUSTRAVELLER_H
#define TUBEWORLD_TORUSTRAVELLER_H


#include "../core/Player.h"
#include "../core/Camera.h"
#include "../core/Shader.h"
#include "../core/lighting/bloom/FBOBloom.h"
#include "../core/Skybox.h"
#include "PerlinLamp.h"
#include "../test-sketches/Sketch01.h"

class TorusTraveller {
private:
public:

    // for drawing scene
    Skybox *skybox;
    PerlinLamp *light;
    Sketch01 *sketch;

    // for fading into and out of island
    GLfloat fadeStep;
    GLfloat fadeTotal;
    glm::vec3 fadeColor;
    GLint fadeStepID;
    GLint fadeTotalID;
    GLint fadeColorID;

    // for post-processing
    FBOBloom *fbo;
    Shader *postShader;

    IOHandler &io;

    // constructor
    TorusTraveller();
    // update scene
    GLint update(Camera &cam, Player &player);
    // draw final torus scene
    void draw();
    // draw torus to offscreen framebuffer
    void renderOffscreen();
    // play with post-processing effects
    void postProcess();
    // clean up VAOs, VBOs, etc.
    void clean();

    // setters
    void setFadeStep(GLfloat fadeStep_) {fadeStep = fadeStep_; };
    void setFadeTotal(GLfloat fadeTotal_) {fadeTotal = fadeTotal_; };

};


#endif //TUBEWORLD_TORUSTRAVELLER_H
