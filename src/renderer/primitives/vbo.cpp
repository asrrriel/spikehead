#include "vbo.h"
#include "third_party/include/glad/glad.h"
#include "GL/gl.h"


VBO::VBO(float* vertices, unsigned int size){
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);    
}
VBO::~VBO(){
    glDeleteBuffers(1, &ID);
}
void VBO::Update(float* vertices, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
}
void VBO::Bind(){
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void VBO::Unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}