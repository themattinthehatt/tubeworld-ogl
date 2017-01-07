//
// Created by mattw on 9/4/16.
//
/*
 * TODO
 * Put lighting functionality in its own class (core/lighting)
 * Use multiple lighting types
 * Play with 3 spheres bouncing around inside the "glass" sphere
 * separate logic from rendering (?)
 * Play with multiple "glass" spheres orbiting a central mass
 * Put it all together!
 */

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "PhysicSpheres.h"
#include "../core/loaders/loadShaders.h"
#include "../core/loaders/loadObjIndexed.h"
#include "../core/vboIndexer.h"

PhysicSpheres::PhysicSpheres()
        :
        io(IOHandler::getInstance()) {


    // -------------------------------------------------------------------------
    //                        Create Spheres info
    // -------------------------------------------------------------------------

    numCenters = 40;
    color = glm::vec3(0.4f, 1.f, 1.f);
    alpha = 1.0f;

    // create and compile our GLSL program from the shaders
    shaderID = loadShaders("src/physic-spheres/SolidShader.vert",
                           "src/physic-spheres/PointLight.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(shaderID, "mMatrix");
    vpMatrixID = glGetUniformLocation(shaderID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(shaderID, "mvpMatrix");
    timeParamID = glGetUniformLocation(shaderID, "time");

    cameraPositionID = glGetUniformLocation(shaderID, "cameraPosition");

    ambientColorID = glGetUniformLocation(shaderID, "material.ambient");
    diffuseColorID = glGetUniformLocation(shaderID, "material.diffuse");
    specularColorID = glGetUniformLocation(shaderID, "material.specular");
    alphaID = glGetUniformLocation(shaderID, "material.alpha");
    shininessID = glGetUniformLocation(shaderID, "material.shininess");

    lightPositionID = glGetUniformLocation(shaderID, "light.position");
    lightDirectionID = glGetUniformLocation(shaderID, "light.direction");
    innerThetaID = glGetUniformLocation(shaderID, "light.innerTheta");
    outerThetaID = glGetUniformLocation(shaderID, "light.outerTheta");
    ambientStrengthID = glGetUniformLocation(shaderID, "light.ambient");
    diffuseStrengthID = glGetUniformLocation(shaderID, "light.diffuse");
    specularStrengthID = glGetUniformLocation(shaderID, "light.specular");
    linID = glGetUniformLocation(shaderID, "light.lin");
    quadID = glGetUniformLocation(shaderID, "light.quad");


    // load sphere model
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadObjIndexed("data/obj/ico_sphere_4.obj",
                              vertices, uvs, normals);

    if (indexVertices) {
        // switch to indexing
        indexVBO(vertices, uvs, normals,
                 indxs,
                 indxVertices, indxUVs, indxNormals);
    } else {
        indxVertices = vertices;
        indxUVs = uvs;
        indxNormals = normals;
    }

    numVerticesPerInstance = static_cast<GLuint>(indxVertices.size());

    // free up memory by swapping vectors with empty temporary vectors
    std::vector<glm::vec3>().swap(vertices);
    std::vector<glm::vec2>().swap(uvs);
    std::vector<glm::vec3>().swap(normals);

    // populate buffer_data
    g_center_buffer_data = new glm::vec3[numCenters];
    g_radial_buffer_data = new glm::vec3[numCenters];
    g_rotation_buffer_data = new glm::mat4[numCenters];

    // get all vertices
    int counter = 0;
    for (int i = 0; i < numCenters; ++i) {
        g_center_buffer_data[counter] = glm::vec3(5.f, static_cast<GLfloat>(i)*4.f, 0.f);
        g_radial_buffer_data[counter] = glm::vec3(0.f);
        g_rotation_buffer_data[counter] = glm::mat4(1.f);
        counter++;
    }

    // -------------------------------------------------------------------------
    //                        Create Spheres VAO
    // -------------------------------------------------------------------------
    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // SPHERE VERTICES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &indxVertices[0], GL_STATIC_DRAW);

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


    // SPHERE NORMALS
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &normalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &indxNormals[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // SPHERE CENTERS
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &centerBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters,
                 g_center_buffer_data, GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
            2,         // attribute 2; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(2, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // RING OFFSET COORDINATES
    // generate 1 buffer, put the resulting identifier in radialBufferID
    glGenBuffers(1, &radialBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, radialBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters,
                 g_radial_buffer_data, GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
            3,         // attribute 3; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(3, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL ROTATIONS
    // generate 1 buffer, put the resulting identifier in rotationBufferID
    glGenBuffers(1, &rotationBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCenters,
                 g_rotation_buffer_data, GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    GLsizei vec4size = sizeof(glm::vec4);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(vec4size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2 * vec4size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3 * vec4size));
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL INDICES
    // generate 1 buffer, put the resulting identifier in elementBufferID
    glGenBuffers(1, &elementBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indxs.size(),
                 &indxs[0], GL_STATIC_DRAW);
    // break buffer binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // unbind the VAO
    glBindVertexArray(0);




    // -------------------------------------------------------------------------
    //                        Create Lamp info
    // -------------------------------------------------------------------------

    lampColor = glm::vec3(1.f, 1.f, 1.f);

    numLamps = 1;

    // create and compile our GLSL program from the shaders
    LshaderID = loadShaders("physic-spheres/LampShader.vert",
                            "physic-spheres/LampShader.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    LmMatrix = glm::mat4(1.0);
    LvpMatrix = glm::mat4(1.0);
    LmvpMatrix = glm::mat4(1.0);
    LmMatrixID = glGetUniformLocation(LshaderID, "mMatrix");
    LvpMatrixID = glGetUniformLocation(LshaderID, "vpMatrix");
    LmvpMatrixID = glGetUniformLocation(LshaderID, "mvpMatrix");
    LtimeParamID = glGetUniformLocation(LshaderID, "time");
    LlampColorID = glGetUniformLocation(LshaderID, "lampColor");

    // load sphere model
    std::vector<glm::vec3> Lvertices;
    std::vector<glm::vec2> Luvs;
    std::vector<glm::vec3> Lnormals;
    bool Lres = loadObjIndexed("data/obj/ico_sphere_4.obj",
                              Lvertices, Luvs, Lnormals);
    // switch to indexing
    indexVBO(Lvertices, Luvs, Lnormals,
             Lindxs,
             LindxVertices, LindxUVs, LindxNormals);
    LnumVerticesPerInstance = static_cast<GLuint>(LindxVertices.size());

    // free up memory by swapping vectors with empty temporary vectors
    std::vector<glm::vec3>().swap(Lvertices);
    std::vector<glm::vec2>().swap(Luvs);
    std::vector<glm::vec3>().swap(Lnormals);

    // get all vertices
    for (int i = 0; i < numLamps; ++i) {
        lampCentersBufferData = glm::vec3(-5.f, static_cast<GLfloat>(i+3)*5.f, 0.f);
    }

    // -------------------------------------------------------------------------
    //                        Create Lamp VAO
    // -------------------------------------------------------------------------
    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &LvertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(LvertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // LAMP MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in LvertexBufferID
    glGenBuffers(1, &LvertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, LvertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * LnumVerticesPerInstance,
                 &LindxVertices[0], GL_STATIC_DRAW);

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


    // LAMP CENTER COORDINATES
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &LcenterBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, LcenterBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numLamps,
                 &lampCentersBufferData[0], GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(1, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL INDICES
    // generate 1 buffer, put the resulting identifier in elementBufferID
    glGenBuffers(1, &LelementBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LelementBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * Lindxs.size(),
                 &Lindxs[0], GL_STATIC_DRAW);
    // break buffer binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // unbind the VAO
    glBindVertexArray(0);


    // -------------------------------------------------------------------------
    //                        Create Translucent Sphere info
    // -------------------------------------------------------------------------

    numTSpheres = 1;
    TScolor = glm::vec3(0.56f, 0.8f, 1.f);
    TSalpha = 0.4;

    // load sphere model
    std::vector<glm::vec3> TSvertices;
    std::vector<glm::vec2> TSuvs;
    std::vector<glm::vec3> TSnormals;
    bool TSres = loadObjIndexed("data/obj/ico_sphere_4.obj",
                                TSvertices, TSuvs, TSnormals);

    if (indexVertices) {
        // switch to indexing
        indexVBO(TSvertices, TSuvs, TSnormals,
                 TSindxs,
                 TSindxVertices, TSindxUVs, TSindxNormals);
    } else {
        TSindxVertices = TSvertices;
        TSindxUVs = TSuvs;
        TSindxNormals = TSnormals;
    }
    for (int i = 0; i < TSindxVertices.size(); ++i) {
        TSindxVertices[i] *= 5.0f;
    }

    numVerticesPerInstance = static_cast<GLuint>(TSindxVertices.size());

    // free up memory by swapping vectors with empty temporary vectors
    std::vector<glm::vec3>().swap(TSvertices);
    std::vector<glm::vec2>().swap(TSuvs);
    std::vector<glm::vec3>().swap(TSnormals);

    // populate buffer_data
    ts_center_buffer_data = new glm::vec3[numTSpheres];
    ts_rotation_buffer_data = new glm::mat4[numTSpheres];

    // get all vertices
    for (int i = 0; i < numTSpheres; ++i) {
        ts_center_buffer_data[i] = glm::vec3(5.f, static_cast<GLfloat>(i+5)*5.f, 0.f);
        ts_rotation_buffer_data[i] = glm::mat4(1.f);
    }

    // -------------------------------------------------------------------------
    //                        Create Translucent Sphere VAO
    // -------------------------------------------------------------------------
    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &TSvertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(TSvertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // SPHERE VERTICES
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &TSvertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, TSvertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &TSindxVertices[0], GL_STATIC_DRAW);

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


    // SPHERE NORMALS
    // generate 1 buffer, put the resulting identifier in vertexBufferID
    glGenBuffers(1, &TSnormalBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, TSnormalBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &TSindxNormals[0], GL_STATIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,         // attribute 1; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0 // array buffer offset
    );
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // SPHERE CENTERS
    // generate 1 buffer, put the resulting identifier in centerBufferID
    glGenBuffers(1, &TScenterBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, TScenterBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numTSpheres,
                 ts_center_buffer_data, GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
            2,         // attribute 2; must match "layout" in shader
            3,         // size (# vertices)
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (GLvoid*)0   // array buffer offset
    );
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(2, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL ROTATIONS
    // generate 1 buffer, put the resulting identifier in rotationBufferID
    glGenBuffers(1, &TSrotationBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, TSrotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numTSpheres,
                 ts_rotation_buffer_data, GL_DYNAMIC_DRAW);

    // set vertex attribute pointers
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(vec4size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2 * vec4size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3 * vec4size));
    // tell OpenGL when to update content of this attribute to next element
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    // break buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // MODEL INDICES
    // generate 1 buffer, put the resulting identifier in elementBufferID
    glGenBuffers(1, &TSelementBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TSelementBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * TSindxs.size(),
                 &TSindxs[0], GL_STATIC_DRAW);
    // break buffer binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // unbind the VAO
    glBindVertexArray(0);

}

/*
 * update buffers with newly created path data
 */
void PhysicSpheres::update(Camera &cam, Player &player) {

    player.update();
    cam.update(player);

    // update sphere info
    time = glfwGetTime();
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;
    cameraPosition = cam.getPosition();

    // update lamp info
    Ltime = glfwGetTime();
    LmMatrix = glm::mat4(1.0);
    LvpMatrix = cam.getProjection() * cam.getView();
    LmvpMatrix = LvpMatrix * LmMatrix;

}

void PhysicSpheres::draw() {

    // -------------------------------------------------------------------------
    //                        Draw Spheres
    // -------------------------------------------------------------------------

    // use our shader (makes programID "currently bound" shader?)
    glUseProgram(shaderID);

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);
    glUniform3fv(cameraPositionID, 1, &cameraPosition[0]);

    glUniform3fv(ambientColorID, 1, &color[0]);
    glUniform3fv(diffuseColorID, 1, &color[0]);
    glUniform3fv(specularColorID, 1, &color[0]);
    glUniform1f(alphaID, alpha);
    glUniform1f(shininessID, 4);

    lampDirection = glm::vec3(1.0f, 0.f, 0.f);
    ambientStrength = glm::vec3(0.1f);
    diffuseStrength = glm::vec3(0.6f);
    specularStrength = glm::vec3(0.4f);
    glUniform3fv(lightPositionID, 1, &lampCentersBufferData[0]);
    glUniform3fv(lightDirectionID, 1, &lampDirection[0]);
    glUniform1f(innerThetaID, glm::cos(glm::radians(45.f)));
    glUniform1f(outerThetaID, glm::cos(glm::radians(55.f)));
    glUniform3fv(ambientStrengthID, 1, &ambientStrength[0]);
    glUniform3fv(diffuseStrengthID, 1, &diffuseStrength[0]);
    glUniform3fv(specularStrengthID, 1, &specularStrength[0]);
    glUniform1f(linID, 0.022);
    glUniform1f(quadID, 0.0019);

    // bind vertex array
    glBindVertexArray(vertexArrayID);

    // send g_center_buffer_data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numCenters,
                 g_center_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // send rotation matrices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, rotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCenters,
                 g_rotation_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (indexVertices) {
        // bind index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

        // draw arrays using currently active shaders
        glDrawElementsInstanced(GL_TRIANGLES, indxs.size(), GL_UNSIGNED_SHORT,
                                0, numCenters);
    } else {
        glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance, numCenters);
    }
    // break vertex array object binding
    glBindVertexArray(0);

    // -------------------------------------------------------------------------
    //                        Draw Lamp
    // -------------------------------------------------------------------------

    // use our shader (makes programID "currently bound" shader?)
    glUseProgram(LshaderID);

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(LmMatrixID, 1, GL_FALSE, &LmMatrix[0][0]);
    glUniformMatrix4fv(LvpMatrixID, 1, GL_FALSE, &LvpMatrix[0][0]);
    glUniformMatrix4fv(LmvpMatrixID, 1, GL_FALSE, &LmvpMatrix[0][0]);
    glUniform1f(LtimeParamID, Ltime);
    glUniform3fv(LlampColorID, 1, &lampColor[0]);

    // bind vertex array
    glBindVertexArray(LvertexArrayID);

    // send lampCentersBufferData to GPU
    glBindBuffer(GL_ARRAY_BUFFER, LcenterBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numLamps,
                 &lampCentersBufferData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LelementBufferID);

    // draw arrays using currently active shaders
    glDrawElementsInstanced(GL_TRIANGLES, Lindxs.size(), GL_UNSIGNED_SHORT, 0, numLamps);
    // break vertex array object binding
    glBindVertexArray(0);

    // -------------------------------------------------------------------------
    //                        Draw Translucent Sphere
    // -------------------------------------------------------------------------

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // use our shader (makes programID "currently bound" shader?)
    glUseProgram(shaderID);

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform1f(timeParamID, time);
    glUniform3fv(cameraPositionID, 1, &cameraPosition[0]);

    glUniform3fv(ambientColorID, 1, &TScolor[0]);
    glUniform3fv(diffuseColorID, 1, &TScolor[0]);
    glUniform3fv(specularColorID, 1, &white[0]);
    glUniform1f(alphaID, TSalpha);
    glUniform1f(shininessID, 4);

    TSambientStrength = glm::vec3(0.1f);
    TSdiffuseStrength = glm::vec3(0.6f);
    TSspecularStrength = glm::vec3(0.4f);
    glUniform3fv(lightPositionID, 1, &lampCentersBufferData[0]);
    glUniform3fv(lightDirectionID, 1, &lampDirection[0]);
    glUniform1f(innerThetaID, glm::cos(glm::radians(45.f)));
    glUniform1f(outerThetaID, glm::cos(glm::radians(55.f)));
    glUniform3fv(ambientStrengthID, 1, &TSambientStrength[0]);
    glUniform3fv(diffuseStrengthID, 1, &TSdiffuseStrength[0]);
    glUniform3fv(specularStrengthID, 1, &TSspecularStrength[0]);
    glUniform1f(linID, 0.022);
    glUniform1f(quadID, 0.0019);


    // bind vertex array
    glBindVertexArray(TSvertexArrayID);

    // send g_center_buffer_data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, TScenterBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numTSpheres,
                 ts_center_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // send rotation matrices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, TSrotationBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numTSpheres,
                 ts_rotation_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TSelementBufferID);

    // draw arrays using currently active shaders
    glDrawElementsInstanced(GL_TRIANGLES, TSindxs.size(), GL_UNSIGNED_SHORT, 0, numTSpheres);
    // break vertex array object binding
    glBindVertexArray(0);

    glDisable(GL_BLEND);

}

void PhysicSpheres::clean() {

    // sphere data
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &centerBufferID);
    glDeleteBuffers(1, &radialBufferID);
    glDeleteBuffers(1, &rotationBufferID);
    glDeleteBuffers(1, &elementBufferID);
    glDeleteProgram(shaderID);

    delete[] g_center_buffer_data;
    delete[] g_radial_buffer_data;
    delete[] g_rotation_buffer_data;

    // lamp data
    glDeleteVertexArrays(1, &LvertexArrayID);
    glDeleteBuffers(1, &LvertexBufferID);
    glDeleteBuffers(1, &LcenterBufferID);
    glDeleteBuffers(1, &LelementBufferID);
    glDeleteProgram(LshaderID);

    // ts data
    glDeleteVertexArrays(1, &TSvertexArrayID);
    glDeleteBuffers(1, &TSvertexBufferID);
    glDeleteBuffers(1, &TSnormalBufferID);
    glDeleteBuffers(1, &TScenterBufferID);
    glDeleteBuffers(1, &TSrotationBufferID);
    glDeleteBuffers(1, &TSelementBufferID);

    delete[] ts_center_buffer_data;
    delete[] ts_rotation_buffer_data;
}