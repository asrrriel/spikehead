#include "ibo.h"
#include "third_party/include/glad/glad.h"
#include "GL/gl.h"
#include <GL/glext.h>


IBO::IBO(unsigned int* indicies, unsigned int size){
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicies, GL_STATIC_DRAW); 
}
IBO::~IBO(){
    glDeleteBuffers(1, &ID);
}
void IBO::Update(unsigned int* indicies, unsigned int size){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indicies);
}
void IBO::Bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}
void IBO::Unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}