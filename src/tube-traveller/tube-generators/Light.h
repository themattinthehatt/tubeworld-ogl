//
// Created by mattw on 12/18/16.
//

#ifndef TUBEWORLD_LIGHT_H
#define TUBEWORLD_LIGHT_H


#include <glm/vec3.hpp>
#include <GL/glew.h>
#include "../../core/Camera.h"
#include "../path-generators/PathGenerator.h"
#include "../../core/Shader.h"

class Light {
private:
public:

    // program ID associated with shader that implements light
    GLuint shaderID;

    // program ID associate with rendering actual light
    Shader *lampShader;

    // define light properties
    glm::vec3 cameraPosition;
    glm::vec3 lightPosition;
    glm::vec3 lightAmbientMax;
    glm::vec3 lightDiffuseMax;
    glm::vec3 lightSpecularMax;
    glm::vec3 lightAmbient;
    glm::vec3 lightDiffuse;
    glm::vec3 lightSpecular;
    GLfloat lightAttLin;
    GLfloat lightAttQuad;
    glm::vec3 lampColorMax;
    glm::vec3 lampColor;
    glm::vec3 lightIntensity;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    GLint numVerticesPerInstance;

    glm::vec3 *g_center_buffer_data;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    GLuint centerBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLint lampColorID;

    // constructor
    Light(GLuint shaderID);
    // update
    void update(const PathGenerator *path, Camera &cam);
    // draw elements
    void draw();
    // draw elements
    void setUniforms();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_LIGHT_H
