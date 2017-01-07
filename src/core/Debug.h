//
// Created by mattw on 8/19/16.
//


#ifndef TUBEWORLD_DEBUG_H
#define TUBEWORLD_DEBUG_H

//#define _GLDEBUG

#include <cstdio>

static void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
//        abort();
    }
}

#ifdef _GLDEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

#endif //TUBEWORLD_DEBUG_H
