//
// Created by mattw on 1/8/17.
//

#ifndef TUBEWORLD_SKETCH01_H
#define TUBEWORLD_SKETCH01_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Skybox.h"
#include "../core/Shader.h"
#include "../core/loaders/loadObj.h"
#include "../core/FramebufferObject.h"

class Sketch01 {

private:


public:

    Skybox *skybox;

    std::vector<glm::vec3> islandCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLint numVertices;

    Shader *shader;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLfloat time;
    GLint timeParamID;

    // for fading into and out of island
    GLfloat fadeStep;
    GLfloat fadeTotal;
    glm::vec3 fadeColor;
    GLint fadeStepID;
    GLint fadeTotalID;
    GLint fadeColorID;

    // for post-processing
    FramebufferObject *fbo;
    Shader *postShader;

    IOHandler &io;

    Sketch01(GLint sketchNumber) : io(IOHandler::getInstance()) {

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

        switch (sketchNumber) {
            case 0:
                // create and compile our GLSL program from the shaders
                shader = new Shader("src/test-sketches/SolidShader.vert",
                                    "src/test-sketches/SolidShaderRed.frag");
                break;
            case 1:
                // create and compile our GLSL program from the shaders
                shader = new Shader("src/test-sketches/SolidShader.vert",
                                    "src/test-sketches/SolidShaderGreen.frag");
                break;
            case 2:
                // create and compile our GLSL program from the shaders
                shader = new Shader("src/test-sketches/SolidShader.vert",
                                    "src/test-sketches/SolidShaderBlue.frag");
                break;
            default:
                // create and compile our GLSL program from the shaders
                shader = new Shader("src/test-sketches/SolidShader.vert",
                                    "src/test-sketches/SolidShaderRed.frag");
                break;
        }

        // -------------------------------------------------------------------------
        //                          Create VAO
        // -------------------------------------------------------------------------

        // load cylinder model
        bool res = loadObj("data/obj/ico_sphere_4.obj",
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
        //                          Initialize uniforms
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

    GLint update(Camera &cam, Player &player) {

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

        if (io.keysPressed[GLFW_KEY_GRAVE_ACCENT]) {
            return 1;
        } else {
            return 0;
        }
    }

    void draw() {

        // render scene to offscreen framebuffer
        renderOffscreen();

        // do final post-processing to color buffer of offscreen framebuffer
        postProcess();

    }

    void renderOffscreen() {

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

    void postProcess() {

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

    void clean() {

        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &normalBufferID);
        glDeleteBuffers(1, &uvBufferID);

        shader->clean();
        delete shader;

        skybox->clean();
        delete skybox;

        fbo->clean();
        delete fbo;

        postShader->clean();
        delete postShader;

    }

    // setters
    void setFadeStep(GLfloat fadeStep_) {fadeStep = fadeStep_; };
    void setFadeTotal(GLfloat fadeTotal_) {fadeTotal = fadeTotal_; };

};


#endif //TUBEWORLD_SKETCH01_H
