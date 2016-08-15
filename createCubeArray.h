//
// Created by mattw on 8/13/16.
//

#ifndef PLAYGROUND_CREATECUBEARRAY_H
#define PLAYGROUND_CREATECUBEARRAY_H
#include <iostream>
// Include GLEW
#include <GL/glew.h>
#include "CubeModelCoordinates.h"
#include "CubeColorCoordinates.h"
#include "CubeUVCoordinates.h"

void createCubeArray(int numCubesX, int numCubesY, int numCubesZ,
                     GLfloat *buffer_data)
{
    int numCubes = numCubesX * numCubesY * numCubesZ;
    int counter = 0;
    float scale = 5.0f;
    GLfloat cubeCenters[numCubes * 3];
    for (int i = 0; i < numCubesX; ++i) {
        for (int j = 0; j < numCubesY; ++j) {
            for (int k = 0; k < numCubesZ; ++k) {
                counter = (i * numCubesY * numCubesZ + j * numCubesZ + k) * 3;
                cubeCenters[counter + 0] = scale * static_cast<float>(i);
                cubeCenters[counter + 1] = scale * static_cast<float>(j);
                cubeCenters[counter + 2] = scale * static_cast<float>(k);
            }
        }
    }

    int fullVertexCount = 0;
    for (int i = 0; i < numCubes; ++i) {
        for (int j = 0; j < numModelVertices; ++j) {
            fullVertexCount = (i * numModelVertices + j) * 3;
            for (int k = 0; k < 3; ++k) {
                buffer_data[fullVertexCount + k] =
                        cubeModelCoordinates[j * 3 + k]
                        + cubeCenters[i * 3 + k];
            }
        }
    }
}

void createColorArray(GLfloat *buffer_data, int numCubes)
{
    int fullVertexCount = 0;
    int modelVertexCount;
    for (int i = 0; i < numCubes; ++i) {
        modelVertexCount = 0;
        for (int j = 0; j < numModelVertices; ++j) {
            fullVertexCount = (i * numModelVertices + j) * 3;
            modelVertexCount = j * 3;
            for (int k = 0; k < 3; ++k) {
                buffer_data[fullVertexCount + k] =
                        cubeColorCoordinates[modelVertexCount + k];
            }
        }
    }

//    for (int i = 0; i < numCubes * numModelVertices * 3; ++i)
//        buffer_data[i] = 1.0f;
}

void createUVArray(GLfloat *buffer_data, int numCubes)
{
    int fullVertexCount = 0;
    int modelVertexCount;
    for (int i = 0; i < numCubes; ++i) {
        modelVertexCount = 0;
        for (int j = 0; j < numModelVertices; ++j) {
            fullVertexCount = (i * numModelVertices + j) * 2;
            modelVertexCount = j * 2;
            for (int k = 0; k < 2; ++k) {
                buffer_data[fullVertexCount + k] =
                        cubeUVCoordinates[modelVertexCount + k];
            }
        }
    }
}

void createSkyboxVertices(GLfloat *buffer_data)
{
//    for (int j = 0; j < numModelVertices; ++j) {
//        for (int k = 0; k < 3; ++k) {
//            buffer_data[3*j + k] =
//                    100.0f * skyboxModelCoordinates[3*j + k];
//        }
//    }
}
#endif //PLAYGROUND_CREATECUBEARRAY_H
