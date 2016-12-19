//
// Created by mattw on 12/18/16.
//

#ifndef TUBEWORLD_TEXTURECYLINDERLIGHT_H
#define TUBEWORLD_TEXTURECYLINDERLIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../core/Global.h"
#include "../../core/Camera.h"
#include "../../core/Shader.h"
#include "../path-generators/PathGenerator.h"
#include "../TubeTraveller.h"

class TextureCylinderLight : public TubeGenerator {
private:
public:

    std::vector<glm::vec3> cylinderModelCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    TextureGenerator *texture;
    Shader *shader;

    GLint numCenters;
    GLint numVerticesPerInstance;
    GLint numCentersPerTexture;
    GLfloat tubeRadius;
    GLfloat tubeLength;

    glm::vec3 **g_center_buffer_data;
    glm::mat4 **g_rotation_buffer_data;
    glm::mat4 **g_rotation2_buffer_data;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    GLuint centerBufferID;
    GLuint rotationBufferID;
    GLuint rotation2BufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLfloat time;
    GLint timeParamID;

    // lighting properties
    glm::vec3 cameraPosition;
    glm::vec3 lightPosition;
    glm::vec3 lightAmbient;
    glm::vec3 lightDiffuse;
    glm::vec3 lightSpecular;
    GLfloat lightAttLin;
    GLfloat lightAttQuad;

    IOHandler &io;

    // constructor
    TextureCylinderLight(GLint numCenters, TubeTraveller::TextureType);
    // update positions/angles of tube elements
    void update(const PathGenerator *path, Camera &cam);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_TEXTURECYLINDERLIGHT_H
