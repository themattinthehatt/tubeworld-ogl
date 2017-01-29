//
// Created by mattw on 1/8/17.
//

#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinBlock.h"

PerlinBlock::PerlinBlock() : io(IOHandler::getInstance()) {

    // create island
    island = new PerlinBlockIsland();

    // create skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files = {"data/textures/sky/front.bmp",
                                      "data/textures/sky/back.bmp",
                                      "data/textures/sky/left.bmp",
                                      "data/textures/sky/right.bmp",
                                      "data/textures/sky/up.bmp",
                                      "data/textures/sky/down.bmp"};
    skybox = new Skybox(files, 1000.0f);

    /*
     * front -> back, rotate cw x 1
     * back -> front, rotate ccw x 1
     * left - fine
     * right - ccw x 2
     * up - cw x 1
     * down - cw x 1
     */

    // create light
    light = new PerlinBlockLight(island->shader->programID);

    // -------------------------------------------------------------------------
    //                          Create portals
    // -------------------------------------------------------------------------

    numSketchPortals = 3;
    numTubePortals = 1;

    // placeholders for island->getPortalInfo return values
    std::vector<glm::vec3> sketchPortalCenters;
    std::vector<glm::vec3> sketchPortalHeadings;
    std::vector<const char*> sketchPortalFileLocs;
    std::vector<glm::vec3> tubePortalCenters;
    std::vector<glm::vec3> tubePortalHeadings;
    std::vector<const char*> tubePortalFileLocs;

    // fill placeholders
    island->getPortalInfo(numSketchPortals, numTubePortals,
                          sketchPortalCenters,
                          sketchPortalHeadings,
                          sketchPortalFileLocs,
                          tubePortalCenters,
                          tubePortalHeadings,
                          tubePortalFileLocs);

    // create sketch portals
    for (int i = 0; i < numSketchPortals; ++i) {
        sketchPortals.push_back(PerlinBlockPortal(
                                    sketchPortalCenters[i],
                                    sketchPortalHeadings[i],
                                    sketchPortalFileLocs[i]));
    }

    // create tube portals
    for (int i = 0; i < numTubePortals; ++i) {
        tubePortals.push_back(PerlinBlockPortal(
                tubePortalCenters[i],
                tubePortalHeadings[i],
                tubePortalFileLocs[i]));
    }

    // -------------------------------------------------------------------------
    //                          Postprocessing
    // -------------------------------------------------------------------------

    fbo  = new FramebufferObject();

    postShader = new Shader("src/island-traveller/perlin-block/PostProcessingShader.vert",
                            "src/island-traveller/perlin-block/PostProcessingShader.frag");

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

GLint PerlinBlock::update(Camera &cam, Player &player) {

    // update player
    player.update();

    // compute view and projection matrices from player info
    cam.update(player);

    // update skybox
    skybox->update(cam);

    // update island
    island->update(cam, player);

    // update light
    light->update(cam);

    // update sketch portals
    for (int i = 0; i < numSketchPortals; ++i) {
        sketchPortals[i].update(cam, player);
    }

    // update tube portals
    for (int i = 0; i < numTubePortals; ++i) {
        tubePortals[i].update(cam, player);
    }


    // determine return condition
    glm::vec3 posPlayer = player.getPosition();
    glm::vec3 headingPlayer = player.getHeading();

    // loop through sketch portals
    for (int i = 0; i < numSketchPortals; ++i) {
        GLfloat dist = glm::distance(posPlayer, sketchPortals[i].center);
        if (dist < 1.f && glm::dot(headingPlayer, sketchPortals[i].heading) < 0.f) {
            return (10 + i);
        }
    }
    // loop through tube portals
    for (int i = 0; i < numTubePortals; ++i) {
        GLfloat dist = glm::distance(posPlayer, tubePortals[i].center);
        if (dist < 1.f && glm::dot(headingPlayer, tubePortals[i].heading) < 0.f) {
            return (1 + i);
        }
    }

    // otherwise, no transition
    return 0;

}

void PerlinBlock::draw() {

    // render scene to offscreen framebuffer
    renderOffscreen();

    // do final post-processing to color buffer of offscreen framebuffer
//    postProcess();

}

void PerlinBlock::renderOffscreen() {

    // FIRST PASS: OFF-SCREEN RENDERING
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo->getFramebufferID());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // now all subsequent rendering operations will render to the attachments of
    // the currently bound framebuffer, but will have no impact on the visual
    // output of the application since we are not rendering to the default
    // framebuffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // no stencil buffer now
    glEnable(GL_DEPTH_TEST);

    // render skybox
    skybox->draw();

    // render lamp
//    light->draw();

    // set uniforms for light
    island->shader->use();
    light->setUniforms();

    // render island
    island->draw();

    // draw sketch portals
    for (int i = 0; i < numSketchPortals; ++i) {
        sketchPortals[i].draw();
    }

    // draw tube portals
    for (int i = 0; i < numTubePortals; ++i) {
        tubePortals[i].draw();
    }

}


void PerlinBlock::postProcess() {

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

void PerlinBlock::clean() {

    skybox->clean();
    delete skybox;

    island->clean();
    delete island;

    light->clean();
    delete light;

    fbo->clean();
    delete fbo;

    postShader->clean();
    delete postShader;

}