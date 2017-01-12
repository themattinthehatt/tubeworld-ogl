//
// Created by mattw on 1/8/17.
//

#include <iostream>
#include <vector>
#include "PerlinBlock.h"
#include "../../core/loaders/loadObj.h"

PerlinBlock::PerlinBlock() : io(IOHandler::getInstance()) {

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

    // create and compile our GLSL program from the shaders
    shader = new Shader("src/island-traveller/perlin-block/SolidShader.vert",
                        "src/island-traveller/perlin-block/SolidShader.frag");

    // -------------------------------------------------------------------------
    //                          Create scene VAO
    // -------------------------------------------------------------------------

    // load cylinder model
    bool res = loadObj("data/obj/cube.obj",
                       islandCoordinates, uvs, normals);
    numVertices = static_cast<GLuint>(islandCoordinates.size());


    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // ISLAND COORDINATES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVertices,
                 &islandCoordinates[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,         // attribute 0; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // ISLAND NORMALS
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &normalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVertices,
                 &normals[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 2; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // ISLAND UV COORDINATES
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &uvBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numVertices,
                 &uvs[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
            2,         // attribute 2; must match "layout" in shader
            2,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbind the VAO
    glBindVertexArray(0);

    // -------------------------------------------------------------------------
    //                          Initialize scene uniforms
    // -------------------------------------------------------------------------

    // vertex shader uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shader->programID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shader->programID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shader->programID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shader->programID, "time");

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

    // update uniforms
    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

    // determine return condition
    glm::vec3 posPlayer = player.getPosition();
    glm::vec3 posPortal = glm::vec3(2.f, 10.f, 2.f);
    GLfloat dist = glm::distance(posPlayer, posPortal);
    if (dist < 2.f) {
        // player is close to block. Where?
        if (posPlayer.x < 2.f && posPlayer.z < 3.f && posPlayer.y > 9.f) {
            // player is on left side - go to sketch 0
            return 10;
        } else if (posPlayer.x > 2.f && posPlayer.z < 3.f && posPlayer.y > 9.f) {
            // player is on right side - go to sketch 1
            return 11;
        } else if (posPlayer.y < 10.f && posPlayer.z < 3.f) {
            // player is in front - go to sketch 2
            return 12;
        } else if (posPlayer.z > 3.f) {
            // player is on top - go to tube
            return 1;
        } else {
            // player is close but not to portal entrance
            return 0;
        }
    } else {
        // player is not close to portal
        return 0;
    }

}

void PerlinBlock::draw() {

    // render scene to offscreen framebuffer
    renderOffscreen();

    // do final post processing to color buffer of offscreen framebuffer
    postProcess();

}

void PerlinBlock::renderOffscreen() {

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

    // use our shader (makes programID currently bound shader)
    shader->use();

    // send data to vertex shader
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    // bind vertex array
    glBindVertexArray(vertexArrayID);
    // draw arrays using currently active shaders
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    // break vertex array object binding
    glBindVertexArray(0);

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

    // scene
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);

    shader->clean();
    delete shader;

    skybox->clean();
    delete skybox;

    postShader->clean();
    delete postShader;

}