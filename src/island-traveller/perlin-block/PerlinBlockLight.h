//
// Created by mattw on 1/29/17.
//

#ifndef TUBEWORLD_PERLINBLOCKLIGHT_H
#define TUBEWORLD_PERLINBLOCKLIGHT_H

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include "../../core/Camera.h"
#include "../../core/Shader.h"

class PerlinBlockLight {
private:
public:

    // program ID associated with shader that implements light
    GLuint shaderID;

    // program ID associate with rendering actual light
    Shader *lampShader;

    enum LightType {
        LIGHTTYPE_POINT,
        LIGHTTYPE_DIR,
        LIGHTTYPE_SPOT,
        MAX_NUM_LIGHTTYPES
    };

    // define light properties
    struct LightProperties {
        LightType type;
        glm::vec3 cameraPosition;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 ambientMax;
        glm::vec3 diffuseMax;
        glm::vec3 specularMax;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat attLin;
        GLfloat attQuad;
        glm::vec3 lampColorMax;
        glm::vec3 lampColor;
        glm::vec3 intensity;
        GLfloat innerTheta;
        GLfloat outerTheta;
    };
    LightProperties lightProps;

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
    PerlinBlockLight(GLuint shaderID);
    // update
    void update(Camera &cam);
    // draw elements
    void draw();
    // draw elements
    void setUniforms();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_PERLINBLOCKLIGHT_H