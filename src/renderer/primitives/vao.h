#pragma once
#include <GL/gl.h>

class VAO {
    unsigned int ID;
public:
    VAO();
    ~VAO();
    void LinkAttrib(GLuint index, GLint size,GLenum type, GLboolean normalized, GLsizei stride, void* pointer);
    void Bind();
    void Unbind();
};