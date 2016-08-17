//
// Created by mattw on 8/16/16.
//
#include <iostream>
#include "CubeArray.h"
#include "../core/loaders/loadShaders.h"
#include "../core/loaders/loadTextures.h"

/*
 * CubeArray()
 * Constructor method for CubeArray class
 */
CubeArray::CubeArray(int numCubesX_, int numCubesY_, int numCubesZ_,
                     bool isTextureRendered_) {

    numCubesX = numCubesX_;
    numCubesY = numCubesY_;
    numCubesZ = numCubesZ_;

    isTextureRendered = isTextureRendered_;
    if (isTextureRendered) {
        // create and compile our GLSL program from the shaders
        shaderID = loadShaders("shaders/TextureShader.vert",
                               "shaders/TextureShader.frag");
    }
    else {
        // create and compile our GLSL program from the shaders
        shaderID = loadShaders("shaders/SolidShader2.vert",
                               "shaders/SolidShader2.frag");
    }

    // give the MVP matrix to GLSL; get a handle on our "MVP" uniform
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    // give time parameter to GLSL
    timeParamID = glGetUniformLocation(shaderID, "time");

    // create VAO for skybox to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers

    // COPY VERTEX ARRAY AND CENTERS ARRAY INTO BUFFERS
    // specify model coordinates
//    int numCubes = (numCubesX * numCubesY * numCubesZ);
//    GLfloat g_vertex_buffer_data[numCubes * numVertices * 3];
//    GLfloat g_center_buffer_data[numCubes * numVertices * 3];
//    // populate buffer_data
//    createCubeArray(numCubesX, numCubesY, numCubesZ, numVertices,
//                    g_vertex_buffer_data, g_center_buffer_data);

    int numCubes = (numCubesX * numCubesZ - (numCubesX - 2) * (numCubesZ - 2))* numCubesY;
    GLfloat g_vertex_buffer_data[numCubes * numVertices * 3];
    GLfloat g_center_buffer_data[numCubes * numVertices * 3];
    // populate buffer_data
    createHollowCubeArray(numCubesX, numCubesY, numCubesZ, numVertices, numCubes,
                          g_vertex_buffer_data, g_center_buffer_data);


    // CUBE MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in VertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
                 g_vertex_buffer_data, GL_STATIC_DRAW);

    // set vertex attribute pointers
    glVertexAttribPointer(
            0,         // attribute 0; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    glEnableVertexAttribArray(0);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // CUBE CENTER COORDINATES
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &centerBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_center_buffer_data),
                 g_center_buffer_data, GL_STATIC_DRAW);

    // set vertex attribute pointers
    glVertexAttribPointer(
            1,         // attribute 0; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    glEnableVertexAttribArray(1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // CUBE COLOR / TEXTURE DATA
    if (isTextureRendered) {
        texture = loadBMP_custom("textures/uvtemplate.bmp");
        // get a handle for our "myTextureSampler" uniform
        textureID = glGetUniformLocation(shaderID, "myTextureSampler");

        // create data to put in UV buffer
        GLfloat g_st_buffer_data[numCubes*numVertices*2];
        createSTArray(numCubes, numVertices, g_st_buffer_data);
        // generate 1 buffer, put the resulting identifier in UVBufferID
        glGenBuffers(1, &stBufferID);
        // bind newly created buffer to GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, stBufferID);
        // copy data into buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_st_buffer_data),
                     g_st_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                0,
                (GLvoid *) 0
        );
        glEnableVertexAttribArray(2);
        // break buffer binding
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // unbind the VAO
    glBindVertexArray(0);
}

/*
 * ~CubeArray()
 * Destructor method for CubeArray class
 */
CubeArray::~CubeArray() {}

/*
 * update()
 * Update position of cube array
 */
void CubeArray::update(const Camera &cam) {
    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.Projection * cam.View;
    mvpMatrix = vpMatrix * mMatrix;
}

/*
 * draw()
 * Draw cube array to screen
 */
void CubeArray::draw() {
    // use our shader (makes programID "currently bound" shader?)
    glUseProgram(shaderID);

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    //glUniform1f(timeParamID, (sinf(time) / 2.0f) + 0.5f);
    glUniform1f(timeParamID, time);

    if (isTextureRendered) {
        // Activate the texture unit first before binding texture
        glActiveTexture(GL_TEXTURE0);
        // binds Texture to the currently active texture unit
        glBindTexture(GL_TEXTURE_2D, texture);
        // puts the texture in texture unit 0
        glUniform1i(textureID, 0);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, Texture2);
        // glUniform1i(TextureID, 1);
    }

    // bind vertex array
    glBindVertexArray(vertexArrayID);
    // draw arrays using currently active shaders
    glDrawArrays(GL_TRIANGLES, 0, numVertices*numCubesX*numCubesY*numCubesZ);
    // break vertex array object binding
    glBindVertexArray(0);
    // break texture binding
    glBindTexture(GL_TEXTURE_2D, 0);

}

/*
 * clean()
 * Clean up VAOs, VBOs, etc.
 */
void CubeArray::clean() {
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteProgram(shaderID);
    if (isTextureRendered) {
        glDeleteTextures(1, &textureID);
        glDeleteBuffers(1, &stBufferID);
    }
    else {
        glDeleteBuffers(1, &colorBufferID);
    }
}

/*
 * createCubeArray()
 * Create initial position for each vertex
 */
void CubeArray::createCubeArray(int numCubesX, int numCubesY, int numCubesZ,
                                int numVertices, GLfloat *vertex_buffer_data,
                                GLfloat *center_buffer_data)
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
        for (int j = 0; j < numVertices; ++j) {
            fullVertexCount = (i * numVertices + j) * 3;
            for (int k = 0; k < 3; ++k) {
                vertex_buffer_data[fullVertexCount + k] =
                        CubeArray::cubeModelCoordinates[j * 3 + k];
                center_buffer_data[fullVertexCount + k] =
                        cubeCenters[i * 3 + k];
            }
        }
    }
}

/*
 * createCubeArray()
 * Create initial position for each vertex
 */
void CubeArray::createHollowCubeArray(int numCubesX, int numCubesY, int numCubesZ,
                                      int numVertices, int numCubes,
                                      GLfloat *vertex_buffer_data,
                                      GLfloat *center_buffer_data)
{
    int counter = 0;
    float scale = 5.0f;
    GLfloat cubeCenters[numCubes * 3];
    for (int i = 0; i < numCubesX; ++i) {
        for (int k = 0; k < numCubesZ; ++k) {
            if (i == 0 || i == numCubesX-1 || k == 0 || k == numCubesZ-1) {
                for (int j = 0; j < numCubesY; ++j) {
                    cubeCenters[counter + 0] =
                            scale * static_cast<float>(i);
                    cubeCenters[counter + 1] =
                            scale * static_cast<float>(j);
                    cubeCenters[counter + 2] =
                            scale * static_cast<float>(k);
                    counter += 3;
                }
            }
        }
    }

    int fullVertexCount = 0;
    for (int i = 0; i < numCubes; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            fullVertexCount = (i * numVertices + j) * 3;
            for (int k = 0; k < 3; ++k) {
                vertex_buffer_data[fullVertexCount + k] =
                        CubeArray::cubeModelCoordinates[j * 3 + k];
                center_buffer_data[fullVertexCount + k] =
                        cubeCenters[i * 3 + k];
            }
        }
    }
}

/*
 * createColorArray()
 * Create initial color data for each vertex
 */
void CubeArray::createColorArray(int numCubes, int numVertices,
                                 GLfloat *buffer_data)
{
    int fullVertexCount = 0;
    int modelVertexCount = 0;
    for (int i = 0; i < numCubes; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            fullVertexCount = (i * numVertices + j) * 3;
            modelVertexCount = j * 3;
            for (int k = 0; k < 3; ++k) {
                buffer_data[fullVertexCount + k] =
                        CubeArray::cubeColorCoordinates[modelVertexCount + k];
            }
        }
    }

//    for (int i = 0; i < numCubes * numModelVertices * 3; ++i)
//        buffer_data[i] = 1.0f;
}

/*
 * createSTArray()
 * Create initial texture coordinate data for each vertex
 */
void CubeArray::createSTArray(int numCubes, int numVertices,
                                    GLfloat *buffer_data)
{
    int fullVertexCount = 0;
    int modelVertexCount = 0;
    for (int i = 0; i < numCubes; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            fullVertexCount = (i * numVertices + j) * 2;
            modelVertexCount = j * 2;
            for (int k = 0; k < 2; ++k) {
                buffer_data[fullVertexCount + k] =
                        CubeArray::cubeSTCoordinates[modelVertexCount + k];
            }
        }
    }
}

GLfloat CubeArray::cubeModelCoordinates[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
         1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end

         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,

         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f
};

GLfloat CubeArray::cubeColorCoordinates[] = {
        0.583f, 1.0f, 1.0f,
        0.609f, 1.0f, 1.0f,
        0.327f, 1.0f, 1.0f,
        0.822f, 1.0f, 1.0f,
        0.435f, 1.0f, 1.0f,
        0.310f, 1.0f, 1.0f,
        0.597f, 1.0f, 1.0f,
        0.559f, 1.0f, 1.0f,
        0.359f, 1.0f, 1.0f,
        0.483f, 1.0f, 1.0f,
        0.559f, 1.0f, 1.0f,
        0.195f, 1.0f, 1.0f,
        0.014f, 1.0f, 1.0f,
        0.771f, 1.0f, 1.0f,
        0.676f, 1.0f, 1.0f,
        0.971f, 1.0f, 1.0f,
        0.140f, 1.0f, 1.0f,
        0.997f, 1.0f, 1.0f,
        0.945f, 1.0f, 1.0f,
        0.543f, 1.0f, 1.0f,
        0.279f, 1.0f, 1.0f,
        0.167f, 1.0f, 1.0f,
        0.347f, 1.0f, 1.0f,
        0.055f, 1.0f, 1.0f,
        0.714f, 1.0f, 1.0f,
        0.783f, 1.0f, 1.0f,
        0.722f, 1.0f, 1.0f,
        0.302f, 1.0f, 1.0f,
        0.225f, 1.0f, 1.0f,
        0.517f, 1.0f, 1.0f,
        0.053f, 1.0f, 1.0f,
        0.393f, 1.0f, 1.0f,
        0.673f, 1.0f, 1.0f,
        0.820f, 1.0f, 1.0f,
        0.982f, 1.0f, 1.0f,
        0.134f, 1.0f, 1.0f
};

GLfloat CubeArray::cubeSTCoordinates[] = {
        0.000059f, 1.0f - 0.000004f,
        0.000103f, 1.0f - 0.336048f,
        0.335973f, 1.0f - 0.335903f,
        1.000023f, 1.0f - 0.000013f,
        0.667979f, 1.0f - 0.335851f,
        0.999958f, 1.0f - 0.336064f,
        0.667979f, 1.0f - 0.335851f,
        0.336024f, 1.0f - 0.671877f,
        0.667969f, 1.0f - 0.671889f,
        1.000023f, 1.0f - 0.000013f,
        0.668104f, 1.0f - 0.000013f,
        0.667979f, 1.0f - 0.335851f,
        0.000059f, 1.0f - 0.000004f,
        0.335973f, 1.0f - 0.335903f,
        0.336098f, 1.0f - 0.000071f,
        0.667979f, 1.0f - 0.335851f,
        0.335973f, 1.0f - 0.335903f,
        0.336024f, 1.0f - 0.671877f,
        1.000004f, 1.0f - 0.671847f,
        0.999958f, 1.0f - 0.336064f,
        0.667979f, 1.0f - 0.335851f,
        0.668104f, 1.0f - 0.000013f,
        0.335973f, 1.0f - 0.335903f,
        0.667979f, 1.0f - 0.335851f,
        0.335973f, 1.0f - 0.335903f,
        0.668104f, 1.0f - 0.000013f,
        0.336098f, 1.0f - 0.000071f,
        0.000103f, 1.0f - 0.336048f,
        0.000004f, 1.0f - 0.671870f,
        0.336024f, 1.0f - 0.671877f,
        0.000103f, 1.0f - 0.336048f,
        0.336024f, 1.0f - 0.671877f,
        0.335973f, 1.0f - 0.335903f,
        0.667969f, 1.0f - 0.671889f,
        1.000004f, 1.0f - 0.671847f,
        0.667979f, 1.0f - 0.335851f
};