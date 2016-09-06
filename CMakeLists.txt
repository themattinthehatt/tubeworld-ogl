cmake_minimum_required(VERSION 3.2)
project(tubeworld)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -g -std=c++11 -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(SOURCE_FILES
        main.cpp
        core/Player.cpp
        core/Camera.cpp
        core/Skybox.cpp
        core/vboIndexer.cpp
        core/loaders/loadShaders.cpp
        core/loaders/loadTextures.cpp
        core/loaders/loadCubemap.cpp
        core/loaders/loadObj.cpp
        core/loaders/loadObjIndexed.cpp
        tube-traveller/TubeTraveller.cpp
        tube-traveller/CubeTube.cpp
        tube-traveller/PathGenerator.h
        tube-traveller/PathCircle.cpp
        tube-traveller/PathUserInput.cpp
        tube-traveller/PathRandom.cpp
        physic-spheres/PhysicSpheres.cpp)


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