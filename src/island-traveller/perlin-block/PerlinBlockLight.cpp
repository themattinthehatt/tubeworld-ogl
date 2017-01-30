//
// Created by mattw on 1/29/17.
//

#include "PerlinBlockLight.h"
#include "../../core/loaders/loadObjIndexed.h"

PerlinBlockLight::PerlinBlockLight(GLuint shaderID_) {

    // program ID associated with shader that implements light
    shaderID = shaderID_;

    // -------------------------------------------------------------------------
    //                    Create light properties
    // -------------------------------------------------------------------------

    // define light properties for ALL types - directional, point, spotlight
    lightProps.type = LIGHTTYPE_DIR;
    lightProps.lampColorMax = glm::vec3(1.0f, 1.0f, 0.8f); // lamp is rendered with this color
    lightProps.cameraPosition = glm::vec3(1.f);

    // for all types
    lightProps.position = glm::vec3(1.f);
    lightProps.direction = glm::vec3(0.f);
    lightProps.ambientMax = glm::vec3(0.8) * lightProps.lampColorMax;
    lightProps.diffuseMax = glm::vec3(1.0) * lightProps.lampColorMax;
    lightProps.specularMax = glm::vec3(0.05);

    // for point and spotlight attenuation
    lightProps.attLin = 0.022f;  // vals from https://learnopengl.com/#!Lighting/Materials
    lightProps.attQuad = 0.0019f;

    // for spotlight
    lightProps.innerTheta = 0.f; // cos of angle
    lightProps.outerTheta = 1.f; // cos of angle
    lightProps.intensity = glm::vec3(1.f);

    // -------------------------------------------------------------------------
    //                        Create lamp
    // -------------------------------------------------------------------------

    // create and compile our GLSL program from the shaders
    lampShader = new Shader("src/tube-traveller/shaders/LampShader.vert",
                            "src/tube-traveller/shaders/LampShader.frag");

    // give the MVP matrix to GLSL; get a handle on our uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = glm::mat4(1.0);
    mvpMatrix = glm::mat4(1.0);
    mMatrixID = glGetUniformLocation(lampShader->programID, "mMatrix");
    vpMatrixID = glGetUniformLocation(lampShader->programID, "vpMatrix");
    mvpMatrixID = glGetUniformLocation(lampShader->programID, "mvpMatrix");
    lampColorID = glGetUniformLocation(lampShader->programID, "lampColor");

    // load sphere model
    bool res = loadObjIndexed("data/obj/ico_sphere_4.obj",
                              vertices, uvs, normals);

    numVerticesPerInstance = static_cast<GLuint>(vertices.size());
    // switch to indexing
//    indexVBO(vertices, uvs, normals,
//             indxs,
//             indxVertices, indxUVs, indxNormals);
//    LnumVerticesPerInstance = static_cast<GLuint>(LindxVertices.size());


    // get all vertices
    g_center_buffer_data = new glm::vec3(0.f);

    // -------------------------------------------------------------------------
    //                        Create Lamp VAO
    // -------------------------------------------------------------------------
    // create VAO to store:
    // - calls to glEnableVertexAttribArray or glDisableVertexAttribArray
    // - vertex attribute configurations via glVertexAttribPointer
    // - VBOs assocatiated with vertex attributes by calls to glVertexAttribPointer
    glGenVertexArrays(1, &vertexArrayID);
    // use a VAO by binding it
    glBindVertexArray(vertexArrayID);
    // from here we bind/config corresponding VBO(s) and attribute pointers


    // LAMP MODEL COORDINATES
    // generate 1 buffer, put the resulting identifier in LvertexBufferID
    glGenBuffers(1, &vertexBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerticesPerInstance,
                 &vertices[0], GL_STATIC_DRAW);

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
    glGenBuffers(1, &centerBufferID);
    // bind newly created buffer to GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3),
                 &g_center_buffer_data[0], GL_DYNAMIC_DRAW);

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


    // unbind the VAO
    glBindVertexArray(0);

    // free up memory by swapping vectors with empty temporary vectors
    std::vector<glm::vec3>().swap(vertices);
    std::vector<glm::vec2>().swap(uvs);
    std::vector<glm::vec3>().swap(normals);

}

// update
void PerlinBlockLight::update(Camera &cam) {

    // update lamp position
    lightProps.cameraPosition = cam.getPosition();
    lightProps.position = glm::vec3(-50.f, 50.f, 50.f);
    lightProps.direction = glm::vec3(1.f, -1.f, -1.f);

    // update buffer that holds lamp position
    g_center_buffer_data[0] = lightProps.position;

    // update light uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

    // update light properties
    lightProps.intensity = glm::vec3(1.f);
    lightProps.ambient = lightProps.ambientMax * lightProps.intensity;
    lightProps.diffuse = lightProps.diffuseMax * lightProps.intensity;
    lightProps.specular = lightProps.specularMax * lightProps.intensity;
    lightProps.lampColor = lightProps.lampColorMax * lightProps.intensity;

}

void PerlinBlockLight::setUniforms() {

    // material properties loaded by PerlinBlockIsland object

    // send properites to shader program
    glUniform3fv(glGetUniformLocation(shaderID, "cameraPosition"),
                 1, &lightProps.cameraPosition[0]);

    switch (lightProps.type) {
        case LIGHTTYPE_DIR:
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.direction"),
                         1, &lightProps.direction[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.specular"),
                         1, &lightProps.specular[0]);
            break;
        case LIGHTTYPE_POINT:
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.position"),
                         1, &lightProps.position[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "pointLight.specular"),
                         1, &lightProps.specular[0]);
            glUniform1f(glGetUniformLocation(shaderID, "pointLight.lin"),
                        lightProps.attLin);
            glUniform1f(glGetUniformLocation(shaderID, "pointLight.quad"),
                        lightProps.attQuad);
            break;
        case LIGHTTYPE_SPOT:
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.position"),
                         1, &lightProps.position[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.direction"),
                         1, &lightProps.direction[0]);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.innerTheta"),
                         lightProps.innerTheta);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.outerTheta"),
                        lightProps.outerTheta);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "spotLight.specular"),
                         1, &lightProps.specular[0]);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.lin"),
                        lightProps.attLin);
            glUniform1f(glGetUniformLocation(shaderID, "spotLight.quad"),
                        lightProps.attQuad);
            break;
        default: // directional light
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.direction"),
                         1, &lightProps.direction[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.ambient"),
                         1, &lightProps.ambient[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.diffuse"),
                         1, &lightProps.diffuse[0]);
            glUniform3fv(glGetUniformLocation(shaderID, "dirLight.specular"),
                         1, &lightProps.specular[0]);
    }

}

// draw lamp
void PerlinBlockLight::draw() {

    // light model properties
    // use our shader (makes programID "currently bound" shader?)
    lampShader->use();

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform3fv(lampColorID, 1, &lightProps.lampColor[0]);

    // bind vertex array
    glBindVertexArray(vertexArrayID);

    // send center buffer data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, centerBufferID);
    // copy data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3),
                 &g_center_buffer_data[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // draw arrays using currently active shaders
    glDrawArraysInstanced(GL_TRIANGLES, 0, numVerticesPerInstance, 1);
    // break vertex array object binding
    glBindVertexArray(0);

}

// clean up VAOs, VBOs, etc.
void PerlinBlockLight::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &centerBufferID);

    delete[] g_center_buffer_data;

    lampShader->clean();
    delete lampShader;

}