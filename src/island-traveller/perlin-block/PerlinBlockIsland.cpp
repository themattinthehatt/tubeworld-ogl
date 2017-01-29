//
// Created by mattw on 1/14/17.
//

#include <vector>
#include "PerlinBlockIsland.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../core/loaders/loadObj.h"
#include "../../../lib/PerlinNoise.h"


PerlinBlockIsland::PerlinBlockIsland() {

    // create and compile our GLSL program from the shaders
    shader = new Shader("src/island-traveller/perlin-block/IslandShader.vert",
                        "src/island-traveller/perlin-block/IslandShaderLight.frag");

    // create material properties for island; interacts with PerlinBlockLight
    islandMaterial.ambient = glm::vec3(0.1f, 0.1f, 0.f);
    islandMaterial.diffuse = glm::vec3(0.1f, 0.1f, 0.f);
    islandMaterial.specular = glm::vec3(1.f, 1.f, 1.f);
    islandMaterial.alpha = 1.f;
    islandMaterial.shininess = 2.f;
    islandMaterial.texUnit = 0;

    // use our shader (makes programID currently bound shader)
    shader->use();

    // send properties as uniforms
    glUniform3fv(glGetUniformLocation(shader->programID, "material.ambient"),
                 1, &islandMaterial.ambient[0]);
    glUniform3fv(glGetUniformLocation(shader->programID, "material.diffuse"),
                 1, &islandMaterial.diffuse[0]);
    glUniform3fv(glGetUniformLocation(shader->programID, "material.specular"),
                 1, &islandMaterial.specular[0]);
    glUniform1f(glGetUniformLocation(shader->programID, "material.alpha"),
                 islandMaterial.alpha);
    glUniform1f(glGetUniformLocation(shader->programID, "material.shininess"),
                islandMaterial.shininess);
    glUniform1i(glGetUniformLocation(shader->programID, "material.loadedTexture"),
                islandMaterial.texUnit);

    // -------------------------------------------------------------------------
    //                        Buffer Data
    // -------------------------------------------------------------------------

    // load block model
    bool res = loadObj("data/obj/cube.obj",
                       blockCoordinates, blockUVs, blockNormals);
    numVerticesPerBlock = static_cast<GLuint>(blockCoordinates.size());

    // scale block model
    blockScale = 0.5f;
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.f),
                                    glm::vec3(blockScale, blockScale, 1.f));
    for (int i = 0; i < numVerticesPerBlock; ++i) {
        blockCoordinates[i] = glm::vec3(
                scaleMat * glm::vec4(blockCoordinates[i], 1.f));
    }

    // get block centers and heights
    initializeIslandCoordinates(bufferDataCentersHeights);
    numBlocks = static_cast<GLint>(bufferDataCentersHeights.size());

    // -------------------------------------------------------------------------
    //                          Create scene VAO
    // -------------------------------------------------------------------------

    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // BLOCK COORDINATES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerBlock,
                 &blockCoordinates[0], GL_STATIC_DRAW);

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


    // BLOCK NORMALS
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &normalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerBlock,
                 &blockNormals[0], GL_STATIC_DRAW);

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


    // BLOCK UV COORDINATES
    // generate 1 buffer, put the resulting identifier in uvBufferID
    glGenBuffers(1, &uvBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numVerticesPerBlock,
                 &blockUVs[0], GL_STATIC_DRAW);

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


    // BLOCK CENTER COORDINATES/HEIGHTS
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &centersHeightsBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centersHeightsBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * numBlocks,
                 &bufferDataCentersHeights[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
            3,         // attribute 1; must match "layout" in shader
            4,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(3, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbind the VAO
    glBindVertexArray(0);

    // clear up memory
    std::vector<glm::vec3>().swap(blockCoordinates);
    std::vector<glm::vec2>().swap(blockUVs);
    std::vector<glm::vec3>().swap(blockNormals);
    std::vector<glm::vec4>().swap(bufferDataCentersHeights);

    // -------------------------------------------------------------------------
    //                          Initialize scene uniforms
    // -------------------------------------------------------------------------

    // vertex shader uniforms
    vpMatrix = glm::mat4(1.0);
    vpMatrixID = glGetUniformLocation(shader->programID, "vpMatrix");
    timeParamID = glGetUniformLocation(shader->programID, "time");

}

void PerlinBlockIsland::update(Camera cam, Player player) {

    // update uniforms
    time = glfwGetTime();
    vpMatrix = cam.getProjection() * cam.getView();

}

void PerlinBlockIsland::draw() {

    // use our shader (makes programID currently bound shader)
    shader->use();

    // send data to vertex shader
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniform1f(timeParamID, time);

    // bind vertex array
    glBindVertexArray(vertexArrayID);
    // draw arrays using currently active shaders
    glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerBlock, numBlocks);
    // break vertex array object binding
    glBindVertexArray(0);

}

void PerlinBlockIsland::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &centersHeightsBufferID);

    shader->clean();
    delete shader;

}

void PerlinBlockIsland::initializeIslandCoordinates(
                    std::vector<glm::vec4> &bufferDataCentersHeights) {

    GLint numXBlocks = 10;
    GLint numYBlocks = 10;

    // set up perlin noise generator
    unsigned int seed = 236;
    PerlinNoise pn = PerlinNoise(seed);
    float scale = 0.1;
    float heightMultiplier = 10.f;
    int numLevels = 10;             // 0 for no leveling

    for (int i = 0; i < numXBlocks; ++i) {
        for (int j = 0; j < numYBlocks; ++j) {

            float xLoc = 5.5f + 1.f*i;
            float yLoc = 5.5f + 1.f*j;

            float height = static_cast<float>(pn.noise(scale*xLoc, scale*yLoc, 0.0));

            // set level
            if (numLevels > 0) {
                height = floor(numLevels * height) / numLevels;
            }

            // load data into buffer
            bufferDataCentersHeights.push_back(
                    glm::vec4(xLoc,
                              yLoc,
                              1.f,  // shift block up by 1 -> (zmin=0, zmax=2)
                              height * heightMultiplier));
        }
    }
}

void PerlinBlockIsland::getPortalInfo(
                   GLint numSketchPortals, GLint numTubePortals,
                   std::vector<glm::vec3> &sketchPortalCenters,
                   std::vector<glm::vec3> &sketchPortalHeadings,
                   std::vector<const char*> &sketchPortalFileLocs,
                   std::vector<glm::vec3> &tubePortalCenters,
                   std::vector<glm::vec3> &tubePortalHeadings,
                   std::vector<const char*> &tubePortalFileLocs) {


    sketchPortalCenters.push_back(glm::vec3(4.95f, 10.f, 1.f));
    sketchPortalCenters.push_back(glm::vec3(15.05f, 10.f, 1.f));
    sketchPortalCenters.push_back(glm::vec3(10.f, 4.95f, 1.f));

    sketchPortalHeadings.push_back(glm::vec3(-1.f, 0.f, 0.f));
    sketchPortalHeadings.push_back(glm::vec3(1.f, 0.f, 0.f));
    sketchPortalHeadings.push_back(glm::vec3(0.f, -1.f, 0.f));

    sketchPortalFileLocs.push_back("data/textures/cave2.bmp");
    sketchPortalFileLocs.push_back("data/textures/cave2.bmp");
    sketchPortalFileLocs.push_back("data/textures/cave2.bmp");

    tubePortalCenters.push_back(glm::vec3(10.f, 10.f, -0.05f));

    tubePortalHeadings.push_back(glm::vec3(0.f, 0.f, -1.f));

    tubePortalFileLocs.push_back("data/textures/temp2.bmp");

}