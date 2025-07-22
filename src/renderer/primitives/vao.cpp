#include "third_party/include/glad/glad.h"
#include "GL/gl.h"
#include "vao.h"

VAO::VAO(){
    glGenVertexArrays(1, &ID);
}
VAO::~VAO(){
    glDeleteVertexArrays(1, &ID);
}
void VAO::LinkAttrib(GLuint index, GLint size,GLenum type, GLboolean normalized, GLsizei stride, void* pointer){
    glBindVertexArray(ID);

    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(index);
}
void VAO::Bind(){
    glBindVertexArray(ID);
}
void VAO::Unbind(){
    glBindVertexArray(0);
}