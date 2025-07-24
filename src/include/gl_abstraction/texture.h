#pragma once

#include <GL/gl.h>

class Texture {
    unsigned int ID;
public:
    Texture(void* data, int width, int height, GLenum filter);
    ~Texture();
    void Bind(int slot = 0);
    void Unbind(int slot = 0);
};