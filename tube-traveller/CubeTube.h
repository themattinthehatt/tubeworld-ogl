//
// Created by mattw on 8/21/16.
//

#ifndef TUBEWORLD_CUBETUBE_H
#define TUBEWORLD_CUBETUBE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "PathUserInput.h"
#include "../core/Camera.h"

class CubeTube {
private:
public:

    const GLfloat PI = 3.14159f;

    std::vector<glm::vec3> cubeModelCoordinates;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    GLuint numVertices;

    int numCubesHorizontal;
    int numCubesVertical;
    int numCenters;
    int numVerticesPerInstance;
    GLuint numModelsPerRing;

    glm::vec3 *ringOffsets;
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

    const bool *keysPressed;    // pointer to keysPressed array
    const bool *keysToggled;    // pointer to keysToggled array

    enum PlayerMode {
        PLAYER_FREE,
        PLAYER_BOUND,
        MAX_PLAYER_MODES
    };
    bool playerModeTrigger = false; // returns true upon release of proper key
    GLuint playerMode = 0;

    // constructor
    CubeTube(GLuint numCubesHorizontal, GLuint numCubesVertical, GLuint numCenters,
             const bool *keysPressed, const bool *keysToggled);
    // update positions/angles of tube elements
    void update(const PathUserInput &path, Player &player, Camera &cam);
    // draw tube elements
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};


#endif //TUBEWORLD_CUBETUBE_H
