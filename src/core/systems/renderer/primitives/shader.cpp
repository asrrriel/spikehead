#include "gl_abstraction/shader.h"
#include "glad/include/glad/glad.h"
#include "GL/gl.h"
#include <iostream>

GLuint __compileShader(GLenum shaderType, const GLchar* source){
    GLuint id = glCreateShader(shaderType);

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success){
        GLchar infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "[ERROR::SHADER::COMPILATION_FAILED] " << infoLog << std::endl;
    }

    return id;
}

Shader::Shader(std::string vert, std::string frag){
    GLuint vertexShader = __compileShader(GL_VERTEX_SHADER, vert.c_str());
    GLuint fragmentShader = __compileShader(GL_FRAGMENT_SHADER, frag.c_str());

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success){
        GLchar infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "[ERROR::SHADER::LINKING_FAILED] " << infoLog << std::endl;
    }

}

void Shader::SetUniform1f(std::string name, float value){
    GLuint loaction = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loaction, value);
}
void Shader::SetUniform3f(std::string name, Vec3f value){
    GLuint loaction = glGetUniformLocation(ID, name.c_str());
    glUniform3f(loaction, value[0], value[1], value[2]);
}

Shader::~Shader(){
    glDeleteProgram(ID);
}
void Shader::Bind(){
    glUseProgram(ID);
}
void Shader::Unbind(){
    glUseProgram(0);
}