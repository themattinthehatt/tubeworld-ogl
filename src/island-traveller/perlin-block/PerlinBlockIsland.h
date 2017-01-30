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

    // for drawing island
    std::vector<glm::vec3> blockCoordinates;
    std::vector<glm::vec2> blockUVs;
    std::vector<glm::vec3> blockNormals;
    GLint numVerticesPerBlock;
    GLint numBlocks;
    GLfloat blockScale;
    std::vector<glm::vec4> bufferDataCentersHeights;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
    GLuint centersHeightsBufferID;
    glm::mat4 vpMatrix;
    GLint vpMatrixID;
    GLfloat time;
    GLint timeParamID;
    GLuint textureID;

    struct material {
        glm::vec3 ambient;  // ambient light color of material (RGB)
        glm::vec3 diffuse;  // diffuse light color of material (RGB)
        glm::vec3 specular; // specular light color of material (RGB)
        float alpha;        // alpha value
        float shininess;    // defines radius of specular highlight
        int texUnit;        // location of texture sampler
    };
    material islandMaterial;

    Shader *shader;

    // for locating portals
    std::vector<glm::vec3> boundaryCenters;
    std::vector<glm::vec3> boundaryNormals;

    // constructor
    PerlinBlockIsland();
    // update island
    void update(Camera cam, Player player);
    // draw island
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

    // initialize island
    void initializeIslandCoordinates(GLfloat &blockScale,
                       std::vector<glm::vec4> &bufferDataCentersHeights);
    // get portal info
    void getPortalInfo(GLint numSketchPortals, GLint numTubePortals,
                       std::vector<glm::vec3> &sketchPortalCenters,
                       std::vector<glm::vec3> &sketchPortalHeadings,
                       std::vector<const char*> &sketchPortalFileLocs,
                       std::vector<glm::vec3> &tubePortalCenters,
                       std::vector<glm::vec3> &tubePortalHeadings,
                       std::vector<const char*> &tubePortalFileLocs);

};
#endif //TUBEWORLD_PERLINBLOCKISLAND_H
