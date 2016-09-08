//
// Created by mattw on 9/4/16.
//

#ifndef TUBEWORLD_PHYSICSPHERES_H
#define TUBEWORLD_PHYSICSPHERES_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../core/Global.h"
#include "../core/Camera.h"

class PhysicSpheres {
private:
public:

    bool indexVertices = true;

    // spheres
    std::vector<GLushort> indxs;
    std::vector<glm::vec3> indxVertices;
    std::vector<glm::vec2> indxUVs;
    std::vector<glm::vec3> indxNormals;

    GLint numCenters;
    GLint numVerticesPerInstance;

    glm::vec3 *g_center_buffer_data;
    glm::vec3 *g_radial_buffer_data;
    glm::mat4 *g_rotation_buffer_data;

    GLfloat sideLength;
    GLfloat spacing;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint centerBufferID;
    GLuint radialBufferID;
    GLuint rotationBufferID;
    GLuint elementBufferID;
    GLuint shaderID;

    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLfloat time;
    glm::vec3 cameraPosition;

    glm::vec3 color;
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    GLfloat alpha;

    glm::vec3 ambientStrength;
    glm::vec3 diffuseStrength;
    glm::vec3 specularStrength;
    GLfloat shininess;


    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLint timeParamID;
    GLint cameraPositionID;

    GLint ambientColorID;
    GLint diffuseColorID;
    GLint specularColorID;
    GLint alphaID;
    GLint shininessID;

    GLint lightPositionID;
    GLint lightDirectionID;
    GLint innerThetaID;
    GLint outerThetaID;
    GLint ambientStrengthID;
    GLint diffuseStrengthID;
    GLint specularStrengthID;
    GLint linID;
    GLint quadID;

    // lamp
    std::vector<GLushort> Lindxs;
    std::vector<glm::vec3> LindxVertices;
    std::vector<glm::vec2> LindxUVs;
    std::vector<glm::vec3> LindxNormals;

    GLint numLamps;
    GLint LnumVerticesPerInstance;

    glm::vec3 lampCentersBufferData;

    GLuint LvertexArrayID;
    GLuint LvertexBufferID;
    GLuint LcenterBufferID;
    GLuint LelementBufferID;
    GLuint LshaderID;

    glm::mat4 LmMatrix;
    glm::mat4 LvpMatrix;
    glm::mat4 LmvpMatrix;
    GLfloat Ltime;
    glm::vec3 lampColor;
    glm::vec3 lampDirection;

    GLint LmMatrixID;
    GLint LvpMatrixID;
    GLint LmvpMatrixID;
    GLint LtimeParamID;
    GLint LlampColorID;

    // translucent sphere
    std::vector<GLushort> TSindxs;
    std::vector<glm::vec3> TSindxVertices;
    std::vector<glm::vec2> TSindxUVs;
    std::vector<glm::vec3> TSindxNormals;

    GLint numTSpheres;
    GLint TSnumVerticesPerInstance;

    glm::vec3 *ts_center_buffer_data;
    glm::mat4 *ts_rotation_buffer_data;

    GLuint TSvertexArrayID;
    GLuint TSvertexBufferID;
    GLuint TSnormalBufferID;
    GLuint TScenterBufferID;
    GLuint TSrotationBufferID;
    GLuint TSelementBufferID;

    glm::vec3 TScolor;
    GLfloat TSalpha;

    glm::vec3 TSambientStrength;
    glm::vec3 TSdiffuseStrength;
    glm::vec3 TSspecularStrength;

    glm::vec3 white = glm::vec3(1.f, 0.f, 1.f);
    glm::vec3 black = glm::vec3(1.f, 0.f, 0.f);

    IOHandler &io;

    // constructor
    PhysicSpheres();
    // update positions/angles of tube elements
    void update(Camera &cam, Player &player);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_PHYSICSPHERES_H
