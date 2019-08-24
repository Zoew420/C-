#pragma once
#include "imgui/glew/GL/glew.h"
#include "imgui/glfw/include/GLFW/glfw3.h"

namespace Simflow {
    using namespace std;

    inline void reset_matrix(int w, int h) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1000, 1000);
        glViewport(0, 0, w, h);
    }

    inline void create_framebuffer(GLuint* framebuffer, GLuint* framebuffer_tex, int w, int h) {
        GLuint fb;
        glGenFramebuffers(1, &fb);
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        // generate texture
        GLuint texColorBuffer;
        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);

        // i32vec2 res = resolution();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // attach it to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        glDeleteFramebuffers(1, framebuffer);
        glDeleteTextures(1, framebuffer_tex);

        *framebuffer = fb;
        *framebuffer_tex = texColorBuffer;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}