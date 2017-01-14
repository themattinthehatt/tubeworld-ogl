//
// Created by mattw on 8/16/16.
//

#include <iostream>
#include "TubeTraveller.h"
#include "path-generators/PathCircle.h"
#include "path-generators/PathUserInput.h"
#include "path-generators/PathRandom.h"
#include "tube-generators/SimpleShapes.h"
#include "tube-generators/TextureCylinderLight.h"

TubeTraveller::TubeTraveller(GLint numCenters,
                             PathGeneratorType pathType,
                             TubeType tubeType,
                             TextureCylinderLight::TextureType textureType,
                             TextureCylinderLight::LightStyle lightStyle)
        : io(IOHandler::getInstance()){

    // create skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files = {"data/textures/stars/front.bmp",
                                      "data/textures/stars/back.bmp",
                                      "data/textures/stars/left.bmp",
                                      "data/textures/stars/right.bmp",
                                      "data/textures/stars/up.bmp",
                                      "data/textures/stars/down.bmp"};
    skybox = new Skybox(files, 1000.0f);

    // select tube type
    GLfloat pathSpacing;
    switch (tubeType) {
        case TUBE_CUBES_SQ:
            tube = new SimpleShapes(numCenters, SimpleShapes::SQUARE_OF_SQUARES);
            pathSpacing = 10.f;
            break;
        case TUBE_CUBES_CIRC:
            tube = new SimpleShapes(numCenters, SimpleShapes::CIRCLE_OF_SQUARES);
            pathSpacing = 20.f;
            break;
        case TUBE_CYLINDER:
            tube = new SimpleShapes(numCenters, SimpleShapes::CYLINDER);
            pathSpacing = 1.8f;
            break;
        case TUBE_TEXTURE_CYLINDER_LIGHT:
            tube = new TextureCylinderLight(numCenters, textureType, lightStyle);
            pathSpacing = 1.8f;
            break;
        default:
            tube = new SimpleShapes(numCenters, SimpleShapes::CIRCLE_OF_SQUARES);
    }

    // select path type
    switch (pathType) {
        case PATH_CIRCLE:
            path = new PathCircle(numCenters, pathSpacing);
            break;
        case PATH_USER:
            path = new PathUserInput(numCenters, pathSpacing);
            break;
        case PATH_RANDOM:
            path = new PathRandom(numCenters, pathSpacing);
            break;
        default:
            path = new PathRandom(numCenters, pathSpacing);
    }

    // -------------------------------------------------------------------------
    //                          Postprocessing
    // -------------------------------------------------------------------------

    fbo  = new FramebufferObject();

    postShader = new Shader("src/tube-traveller/shaders/PostProcessingShader.vert",
                            "src/tube-traveller/shaders/PostProcessingShader.frag");

    // post processing uniforms
    fadeStep = 0.f;
    fadeTotal = 1.f;
    fadeColor = glm::vec3(0.f, 0.f, 0.f);
    fadeStepID = glGetUniformLocation(postShader->programID, "fadeStep");
    fadeTotalID = glGetUniformLocation(postShader->programID, "fadeTotal");
    fadeColorID = glGetUniformLocation(postShader->programID, "targetColor");
    glUniform3fv(fadeColorID, 1, &fadeColor[0]);
    glUniform1f(fadeStepID, fadeStep);
    glUniform1f(fadeTotalID, fadeTotal);

}

void TubeTraveller::update(Camera &cam, Player &player) {

    // toggle camera mode
    if (io.keysToggled[GLFW_KEY_SPACE] != playerModeTrigger) {
        playerModeTrigger = !playerModeTrigger;
        playerMode = (playerMode + 1) % MAX_PLAYER_MODES;
    }
    switch (playerMode) {
        case PLAYER_BOUND:
            // path class updates player
            path->update(player);
            break;
        case PLAYER_FREE:
            // user input updates player
            player.update();
            break;
        default:
            player.update();
    }

    // compute view and projection matrices from player info
    cam.update(player);

    // update tube
    tube->update(path, cam);

    // update skybox
    skybox->update(cam);
}

void TubeTraveller::draw() {

    // render scene to offscreen framebuffer
    renderOffscreen();

    // do final post-processing to color buffer of offscreen framebuffer
    postProcess();

}

void TubeTraveller::renderOffscreen() {

    // set up framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->getFramebufferID());
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // draw
    skybox->draw();
    tube->draw();

}

void TubeTraveller::postProcess() {

    // SECOND PASS: ON-SCREEN RENDERING
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // now all subsequent rendering operations will render to the attachments of
    // the default framebuffer, and as such will be rendered to the scene
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // use post-processing shader
    postShader->use();

    // send uniforms to fragment shader
    glUniform3fv(fadeColorID, 1, &fadeColor[0]);
    glUniform1f(fadeStepID, fadeStep);
    glUniform1f(fadeTotalID, fadeTotal);

    // render fbo color texture attachment to screen
    fbo->render();

}

void TubeTraveller::clean() {

    skybox->clean();
    delete skybox;

    path->clean();
    delete path;

    tube->clean();
    delete tube;

    fbo->clean();
    delete fbo;

    postShader->clean();
    delete postShader;

}