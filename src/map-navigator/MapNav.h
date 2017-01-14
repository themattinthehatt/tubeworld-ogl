//
// Created by mattw on 1/7/17.
//

#ifndef TUBEWORLD_MAPNAV_H
#define TUBEWORLD_MAPNAV_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../core/Skybox.h"
#include "../tube-traveller/TubeTraveller.h"
#include "../island-traveller/IslandTraveller.h"

class MapNav {
private:

public:

    TubeTraveller *tube;
    IslandTraveller *island;

    // keep track of time changes
    GLfloat timePrevious;
    GLfloat timeCurrent;
    GLfloat timeDelta;

    // set up island info
    bool renderIsland;
    bool stopIslandFlag;

    // set up tube info
    bool renderTubeFadeIn;
    bool renderTubeNormal;
    bool renderTubeFadeOut;
    bool renderTubeFinal;
    bool renderTube;
    GLfloat tubeFadeInTime;
    GLfloat tubeFadeOutTime;
    GLfloat tubeTotalTime;
    GLfloat tubeTimeCumulative;
    bool stopTubeFlag;

    // constructor
    MapNav();
    // update dynamics of island
    void update(Camera &cam, Player &player);
    // draw island
    void draw();
    // clean up VAOs, VBOs, etc.
    void clean();

};

#endif //TUBEWORLD_MAPNAV_H