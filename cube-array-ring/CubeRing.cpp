//
// Created by mattw on 8/20/16.
//

#include "CubeRing.h"

CubeRing::CubeRing(int numCubesHorizontal, int numCubesVertical, glm::vec3 coords[]) {

    numModels = 2 * numCubesHorizontal + 2 * (numCubesVertical - 2);
    glm::vec3 modelOffsets[numModels];
    normalVec = glm::vec3(0.0f, 1.0f, 0.0f);

    GLfloat sideLength = 1.0f;
    GLfloat spacing = 5.0f;

    // get centers
    int counter = 0;
    for (int i = 0; i < numCubesHorizontal; ++i) {
        for (int j = 0; j < numCubesVertical; ++j) {
            if (i == 0 || i == numCubesHorizontal-1 || j == 0 || j == numCubesVertical-1) {
                    modelOffsets[counter] = glm::vec3(
                            spacing * static_cast<float>(-numCubesHorizontal/2 + i),
                            spacing * static_cast<float>(-numCubesVertical/2 + j),
                            0);
                    counter++;
            }
        }
    }

    // get all vertices
    counter = 0;
    for (int i = 0; i < numModels; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            coords[counter] = modelOffsets[i] + sideLength * cubeModelCoordinates[j];
        }
    }



}

glm::vec3 CubeRing::cubeModelCoordinates[] = {
        glm::vec3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f), // triangle 1 : end
        glm::vec3(1.0f, 1.0f,-1.0f), // triangle 2 : begin
        glm::vec3(-1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f), // triangle 2 : end

        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f, 1.0f,-1.0f),
        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),

        glm::vec3(-1.0f,-1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f,-1.0f),

        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f,-1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f, 1.0f,-1.0f),

        glm::vec3(1.0f,-1.0f,-1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),

        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f,-1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f,-1.0f, 1.0f)
};
