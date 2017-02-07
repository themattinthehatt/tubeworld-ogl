//
// Created by mattw on 2/5/17.
//

#include <GL/glew.h>
#include "TorusTraveller.h"

TorusTraveller::TorusTraveller() : io(IOHandler::getInstance()) {

    // create skybox; +x, -x, +y, -y, +z, -z
    std::vector<const char*> files = {"data/textures/stars/front.bmp",
                "data/textures/stars/back.bmp",
                "data/textures/stars/left.bmp",
                "data/textures/stars/right.bmp",
                "data/textures/stars/up.bmp",
                "data/textures/stars/down.bmp"};
    skybox = new Skybox(files, 1000.0f);

    /*
     * front -> back, rotate cw x 1
     * back -> front, rotate ccw x 1
     * left - fine
     * right - ccw x 2
     * up - cw x 1
     * down - cw x 1
     */

    sketch = new Sketch01(1);

    // -------------------------------------------------------------------------
    //                          Postprocessing
    // -------------------------------------------------------------------------

    GLint numSamples = 4;
    fbo = new FramebufferObject(numSamples);

    postShader = new Shader("src/torus-traveller/PostShader.vert",
                            "src/torus-traveller/PostShader.frag");

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

    light = new PerlinLamp(sketch->shader->programID, fbo->framebufferID);

}

GLint TorusTraveller::update(Camera &cam, Player &player) {

    // update player
    player.update();

    // compute view and projection matrices from player info
    cam.update(player);

    // update skybox
    skybox->update(cam);

    // update light
    light->update(cam);

//    if (io.keysPressed[GLFW_KEY_GRAVE_ACCENT]) {
//        return 1;
//    } else {
//        return 0;
//    }
}

void TorusTraveller::draw() {

    // render scene to offscreen framebuffer
    renderOffscreen();

    // do final post-processing to color buffer of offscreen framebuffer
    postProcess();

}

void TorusTraveller::renderOffscreen() {

    // FIRST PASS: OFF-SCREEN RENDERING
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->getFramebufferID());
    // now all subsequent rendering operations will render to the attachments of
    // the currently bound framebuffer, but will have no impact on the visual
    // output of the application since we are not rendering to the default
    // framebuffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // no stencil buffer now
    glEnable(GL_DEPTH_TEST);

    // render skybox
    skybox->draw();

    // render light
    light->draw();

}

void TorusTraveller::postProcess() {

    // SECOND PASS: ON-SCREEN RENDERING
    fbo->transferMSFBO();

    // use post-processing shader
    postShader->use();

    // send uniforms to fragment shader
    glUniform3fv(fadeColorID, 1, &fadeColor[0]);
    glUniform1f(fadeStepID, fadeStep);
    glUniform1f(fadeTotalID, fadeTotal);

    // render fbo color texture attachment to screen
    fbo->render();

}

void TorusTraveller::clean() {

    skybox->clean();
    delete skybox;

    light->clean();
    delete light;

    fbo->clean();
    delete fbo;

    postShader->clean();
    delete postShader;

    sketch->clean();
    delete sketch;

}