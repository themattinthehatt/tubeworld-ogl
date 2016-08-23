//
// Created by mattw on 8/22/16.
//

#ifndef TUBEWORLD_LOADOBJ_H
#define TUBEWORLD_LOADOBJ_H

#include <vector>

bool loadObj(
        const char *path,
        std::vector<glm::vec3> &out_vertices,
        std::vector<glm::vec2> &out_uvs,
        std::vector<glm::vec3> &out_normals
);

#endif //TUBEWORLD_LOADOBJ_H
