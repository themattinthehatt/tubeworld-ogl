//
// Created by mattw on 12/18/16.
//

#include "Light.h"
#include "../../core/loaders/loadObjIndexed.h"

Light::Light(GLuint shaderID_) {

    // program ID associated with shader that implements light
    shaderID = shaderID_;

    // define light properties
    lampColorMax = glm::vec3(1.0f, 1.0f, 0.8f); // lamp is rendered with this color
    cameraPosition = glm::vec3(1.f);
    lightPosition = glm::vec3(1.f);
    lightAmbientMax = glm::vec3(1.0) * lampColorMax;
    lightDiffuseMax = glm::vec3(1.0) * lampColorMax;
    lightSpecularMax = glm::vec3(1.0);
    lightAttLin = 0.022f;  // vals from https://learnopengl.com/#!Lighting/Materials
    lightAttQuad = 0.0019f;
    lightIntensity = glm::vec3(1.f);

    // -------------------------------------------------------------------------
    //                        Create light info
    // -------------------------------------------------------------------------

    // create and compile our GLSL program from the shaders
    lampShader = new Shader("tube-traveller/shaders/LampShader.vert",
                            "tube-traveller/shaders/LampShader.frag");

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

    // free up memory by swapping vectors with empty temporary vectors
//    std::vector<glm::vec3>().swap(vertices);
//    std::vector<glm::vec2>().swap(uvs);
//    std::vector<glm::vec3>().swap(normals);

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


}

// update
void Light::update(const PathGenerator *path, Camera &cam) {

    // update lamp position
    cameraPosition = cam.getPosition();
    int offset = 50;
    lightPosition = (1-path->propOfPathTraveled) *
                    path->positions[((path->firstElement)+offset) % path->numCenters] +
                    path->propOfPathTraveled *
                    path->positions[((path->firstElement)+1+offset) % path->numCenters];

    // update buffer that holds lamp position
    g_center_buffer_data[0] = lightPosition;

    // update light uniforms
    mMatrix = glm::mat4(1.0);
    vpMatrix = cam.getProjection() * cam.getView();
    mvpMatrix = vpMatrix * mMatrix;

    // update light intensity
//    lightIntensity = glm::vec3(1.f);
    lightIntensity = glm::vec3(0.7f + 0.3*sin(glfwGetTime()));
    lightAmbient = lightAmbientMax * lightIntensity;
    lightDiffuse = lightDiffuseMax * lightIntensity;
    lightSpecular = lightSpecularMax * lightIntensity;
    lampColor = lampColorMax * lightIntensity;

}

void Light::setUniforms() {

    // material properties loaded by TextureGenerator object

    // light properites
    glUniform3fv(glGetUniformLocation(shaderID, "cameraPosition"),
                 1, &cameraPosition[0]);
    glUniform3fv(glGetUniformLocation(shaderID, "pointLight.position"),
                 1, &lightPosition[0]);
    glUniform3fv(glGetUniformLocation(shaderID, "pointLight.ambient"),
                 1, &lightAmbient[0]);
    glUniform3fv(glGetUniformLocation(shaderID, "pointLight.diffuse"),
                 1, &lightDiffuse[0]);
    glUniform3fv(glGetUniformLocation(shaderID, "pointLight.specular"),
                 1, &lightSpecular[0]);
    glUniform1f(glGetUniformLocation(shaderID, "pointLight.lin"),
                lightAttLin);
    glUniform1f(glGetUniformLocation(shaderID, "pointLight.quad"),
                lightAttQuad);
}

// draw lamp
void Light::draw() {

    // light model properties
    // use our shader (makes programID "currently bound" shader?)
    lampShader->use();

    // send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix
    glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniform3fv(lampColorID, 1, &lampColor[0]);

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
void Light::clean() {

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &centerBufferID);

    delete[] g_center_buffer_data;

    lampShader->clean();
    delete lampShader;

}