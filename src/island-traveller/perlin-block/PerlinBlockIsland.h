//
// Created by mattw on 1/14/17.
//

#ifndef TUBEWORLD_PERLINBLOCKISLAND_H
#define TUBEWORLD_PERLINBLOCKISLAND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../core/Shader.h"
#include "../../core/Player.h"
#include "../../core/Camera.h"

class PerlinBlockIsland {
private:
public:

    std::vector<glm::vec3> islandCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLint numVertices;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    glm::mat4 mMatrix;
    glm::mat4 vpMatrix;
    glm::mat4 mvpMatrix;
    GLint mMatrixID;
    GLint vpMatrixID;
    GLint mvpMatrixID;
    GLfloat time;
    GLint timeParamID;

    Shader *shader;

    // constructor
    PerlinBlockIsland();
    // update island
    void update(Camera cam, Player player);
    // draw island
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

    void getPortalInfo(GLint numSketchPortals, GLint numTubePortals,
                       std::vector<glm::vec3> &sketchPortalCenters,
                       std::vector<glm::vec3> &sketchPortalHeadings,
                       std::vector<const char*> &sketchPortalFileLocs,
                       std::vector<glm::vec3> &tubePortalCenters,
                       std::vector<glm::vec3> &tubePortalHeadings,
                       std::vector<const char*> &tubePortalFileLocs);

};
#endif //TUBEWORLD_PERLINBLOCKISLAND_H
