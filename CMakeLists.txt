cmake_minimum_required(VERSION 3.2)
project(tubeworld)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -g -std=c++11 -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(SOURCE_FILES
        main.cpp
        lib/PerlinNoise.cpp
        src/core/Player.cpp
        src/core/Camera.cpp
        src/core/FramebufferObject.cpp
        src/core/Skybox.cpp
        src/core/vboIndexer.cpp
        src/core/loaders/loadShaders.cpp
        src/core/loaders/loadTextures.cpp
        src/core/loaders/loadCubemap.cpp
        src/core/loaders/loadObj.cpp
        src/core/loaders/loadObjIndexed.cpp
        src/map-navigator/MapNav.cpp
        src/tube-traveller/TubeTraveller.cpp
        src/tube-traveller/path-generators/PathCircle.cpp
        src/tube-traveller/path-generators/PathUserInput.cpp
        src/tube-traveller/path-generators/PathRandom.cpp
        src/tube-traveller/tube-generators/SimpleShapes.cpp
        src/tube-traveller/tube-generators/TextureCylinderLight.cpp
        src/tube-traveller/tube-generators/Light.cpp
        src/tube-traveller/texture-generators/StaticFiles.cpp
        src/tube-traveller/texture-generators/Rainbow.cpp
        src/tube-traveller/texture-generators/Binary.cpp
        src/tube-traveller/texture-generators/Noise.cpp
        src/physic-spheres/PhysicSpheres.cpp
        src/island-traveller/IslandTraveller.cpp
        src/island-traveller/perlin-block/PerlinBlock.cpp
        src/island-traveller/perlin-block/PerlinBlockPortal.cpp
        src/island-traveller/perlin-block/PerlinBlockIsland.cpp
        src/island-traveller/perlin-block/PerlinBlockLight.cpp
        src/torus-traveller/TorusTraveller.cpp
        src/torus-traveller/PerlinLamp.cpp)


add_executable(tubeworld ${SOURCE_FILES})

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

include_directories(
        ${GLEW_INCLUDE_DIRS}
        ${OpenGL_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(tubeworld
        ${GLEW_LIBRARIES}
        ${OpenGL_LIBRARIES}
        ${GLFW_STATIC_LIBRARIES}
)