//
// Created by mattw on 12/3/16.
//

#ifndef TUBEWORLD_TEXTURECYLINDER_H
#define TUBEWORLD_TEXTURECYLINDER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../core/Global.h"
#include "../../core/Camera.h"
#include "../path-generators/PathGenerator.h"

class TextureCylinder : public TubeGenerator {
private:
public:

    std::vector<glm::vec3> cylinderModelCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLint numVertices;

    GLint numCenters;
    GLint numVerticesPerInstance;
    GLint numModelsPerRing;

    glm::vec3 *ringCenters;
    glm::vec3 *g_center_buffer_data;
    glm::vec3 *modelOffsets;
    glm::vec3 *g_radial_buffer_data;
    glm::mat4 *rotationMatrix;
    glm::mat4 *g_rotation_buffer_data;

    GLfloat sideLength;
    GLfloat spacing;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint centerBufferID;
    GLuint radialBufferID;
    GLuint rotationBufferID;
    GLuint shaderID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLuint mMatrixID;
    GLuint vpMatrixID;
    GLuint mvpMatrixID;
    GLfloat time;
    GLuint timeParamID;

    IOHandler &io;

    enum RingModelType {
        SQUARE_OF_SQUARES,
        CIRCLE_OF_SQUARES,
        CYLINDER,
    };
    RingModelType ringModelType;

    // constructor
    TextureCylinder(GLint numCenters, RingModelType ringModelType);
    // update positions/angles of tube elements
    void update(const PathGenerator *path, Camera &cam);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_TEXTURECYLINDER_H
