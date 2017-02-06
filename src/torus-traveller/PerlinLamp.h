//
// Created by mattw on 2/5/17.
//

#ifndef TUBEWORLD_PERLINLAMP_H
#define TUBEWORLD_PERLINLAMP_H

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include "../core/Camera.h"
#include "../core/Shader.h"

class PerlinLamp {
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

    // other light info
    bool changeIntensity;
    bool changeHue;
    bool useIndexing;

    // information for rendering physical lamp
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    GLint numVerticesPerInstance;
    GLint numIndxsPerElement;

    // buffer IDs
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    GLuint elementBufferID;

    // texture IDs
    GLuint textureID;
    GLint samplerID;

    // uniforms and their values
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLfloat threshold;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLint thresholdID;
    GLint lampColorID;

    // constructor
    PerlinLamp(GLuint shaderID);
    // update
    void update(Camera &cam);
    // draw elements
    void draw();
    // draw elements
    void setUniforms();
    // clean up VAOs, VBOs, etc.
    void clean();
    // generate perlin noise texture for sampling
    void generateTexture();

};


#endif //TUBEWORLD_PERLINLAMP_H