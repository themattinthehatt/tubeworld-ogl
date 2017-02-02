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

    // -------------------------------------------------------------------------
    //                 Create material properties for island
    // -------------------------------------------------------------------------
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

    // make texture
    int height = 8;
    int width = 8;
    GLubyte image[height][width][4];

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (j < 2) {
                // top; green
                image[i][j][0] = (GLubyte) 8;
                image[i][j][1] = (GLubyte) 54;
                image[i][j][2] = (GLubyte) 4;
                image[i][j][3] = (GLubyte) 255;
            } else {
                // bottom; brown
                // top; green
                image[i][j][0] = (GLubyte) 26; //179
                image[i][j][1] = (GLubyte) 13; //89
                image[i][j][2] = (GLubyte) 0;
                image[i][j][3] = (GLubyte) 255;
            }
        }
    }

    // send texture to texture unit
    // generate 1 texture ID, put the resutling identifier in textureID
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    // bind newly created texture to GL_TEXTURE_2D target; subsequent texture
    // commands will configure the currently bound texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // generate the texture by using the previously loaded image data
    glTexImage2D(
            GL_TEXTURE_2D,     // texture target; will gen texture on textureIDs[i]
            0,                 // mipmap level; use base of 0
            GL_RGBA,           // type of format we want to store the texture
            width,
            height,
            0,                 // legacy bs
            GL_RGBA,           // format of source image
            GL_UNSIGNED_BYTE,  // format of source image
            image              // source image
    );
    // textureIDs[i] now has texture image attached to it
    // glGenerateMipmap here if desired

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // puts the texture in texture unit 0
    glUniform1i(glGetUniformLocation(shader->programID, "material.loadedTexture"),
                islandMaterial.texUnit);

    // unbind the texture object
    glBindTexture(GL_TEXTURE_2D, 0);

    // -------------------------------------------------------------------------
    //                        Buffer Data
    // -------------------------------------------------------------------------

    // get block centers and heights
    initializeIslandCoordinates(blockScale, bufferDataCentersHeights);
    numBlocks = static_cast<GLint>(bufferDataCentersHeights.size());

    // load block model
    bool res = loadObj("data/obj/cube2.obj",
                       blockCoordinates, blockUVs, blockNormals);
    numVerticesPerBlock = static_cast<GLuint>(blockCoordinates.size());

    // scale block model
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.f),
                                    glm::vec3(blockScale, blockScale, 1.f));
    for (int i = 0; i < numVerticesPerBlock; ++i) {
        blockCoordinates[i] = glm::vec3(
                scaleMat * glm::vec4(blockCoordinates[i], 1.f));
    }


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

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    // bind texture to the currently active texture unit
    glBindTexture(GL_TEXTURE_2D, textureID);

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
    glDeleteTextures(1, &textureID);

    shader->clean();
    delete shader;

}

void PerlinBlockIsland::initializeIslandCoordinates(GLfloat &blockScale,
                    std::vector<glm::vec4> &bufferDataCentersHeights) {

    blockScale = 0.51f;
    GLint numXBlocks = 100;
    GLint numYBlocks = 100;

    // set up perlin noise generator
    unsigned int seed = 236;
    PerlinNoise pn = PerlinNoise(seed);
    float scale = 0.05;                 // scale of perlin noise space to sample
    float heightMultiplier = 10.f;      // higher numbers result in more cliffs
    int numLevels = 0;                  // 0 for no leveling

    for (int i = 0; i < numXBlocks; ++i) {
        for (int j = 0; j < numYBlocks; ++j) {

            float xLoc = 5.5f + 1.f*i;
            float yLoc = 5.5f + 1.f*j;

            float height = static_cast<float>(pn.noise(scale*xLoc, scale*yLoc, 0.0));

            // set level
            if (numLevels > 0) {
                height = floor(numLevels * height) / numLevels;
            }

            if (height > 0.5) {
                heightMultiplier = 12.f;
            } else {
                heightMultiplier = 10.f;
            }

            // load data into buffer
            bufferDataCentersHeights.push_back(
                    glm::vec4(xLoc,
                              yLoc,
                              1.f,  // shift block up by 1 -> (zmin=0, zmax=2)
                              height * heightMultiplier));
        }
    }

    boundaryCenters.push_back(glm::vec3(4.95f, 10.f, 1.f));
    boundaryCenters.push_back(glm::vec3(5.55f + numXBlocks, 10.f, 1.f));
    boundaryCenters.push_back(glm::vec3(10.f, 4.95f, 1.f));
    boundaryCenters.push_back(glm::vec3(10.f, 10.f, -0.05f));

    boundaryNormals.push_back(glm::vec3(-1.f, 0.f, 0.f));
    boundaryNormals.push_back(glm::vec3(1.f, 0.f, 0.f));
    boundaryNormals.push_back(glm::vec3(0.f, -1.f, 0.f));
    boundaryNormals.push_back(glm::vec3(0.f, 0.f, -1.f));

}

void PerlinBlockIsland::getPortalInfo(
                   GLint numSketchPortals, GLint numTubePortals,
                   std::vector<glm::vec3> &sketchPortalCenters,
                   std::vector<glm::vec3> &sketchPortalHeadings,
                   std::vector<const char*> &sketchPortalFileLocs,
                   std::vector<glm::vec3> &tubePortalCenters,
                   std::vector<glm::vec3> &tubePortalHeadings,
                   std::vector<const char*> &tubePortalFileLocs) {

    // randomly choose locations for sketch portals from boundary cubes
    for (int i = 0; i < numSketchPortals; ++i) {
        sketchPortalCenters.push_back(boundaryCenters[i]);
        sketchPortalHeadings.push_back(boundaryNormals[i]);
        sketchPortalFileLocs.push_back("data/textures/cave1.bmp");
    }

    // randomly choose locations for tube portals from boundary cubes
    for (int i = numSketchPortals; i < numSketchPortals + numTubePortals; ++i) {
        tubePortalCenters.push_back(boundaryCenters[i]);
        tubePortalHeadings.push_back(boundaryNormals[i]);
        tubePortalFileLocs.push_back("data/textures/cave1.bmp");
    }

    // clear up memory - boundary info no longer needed
    std::vector<glm::vec3>().swap(boundaryCenters);
    std::vector<glm::vec3>().swap(boundaryNormals);

}