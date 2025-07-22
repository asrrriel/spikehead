#include "platform.h"
#include "third_party/include/glad/glad.h"
#include "GL/gl.h"
#include "renderer.h"
#include "primitives/vbo.h"
#include "primitives/vao.h"
#include "primitives/ibo.h"
#include <cstdint>
#include <iostream>
#include <sys/types.h>

platform_gl_context_t gl_context;

void __renderer_print_errors(){
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error: " << err << "\n";
    }
}

bool renderer_init(platform_context_t context, platform_window_t window){
    gl_context = platform_create_gl_context(context, window);
    platform_make_context_current(gl_context);

    if(!gladLoadGL()){
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    const char* version = (const char*)glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << '\n'; 

    __renderer_print_errors();

    return true;
}
bool renderer_setbgcol(float r, float g, float b){
    glClearColor(r, g, b, 1.0f);
    __renderer_print_errors();
    return true;
}
bool renderer_clear(){
    glClear(GL_COLOR_BUFFER_BIT);
    __renderer_print_errors();
    return true;
}

void renderer_swap(){
    platform_swap_buffers(gl_context);
}

void renderer_deinit(){
    platform_destroy_gl_context(gl_context);
}

struct renderer_object_t {
    VAO* vao;
    IBO* ibo;
    GLuint shader;
    uint32_t num_ind;
    texture_t texture;
};

GLuint __compile_shader(GLenum type,std::string code){
    GLuint shader = glCreateShader(type);
    const char* src = code.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile failed: " << infoLog << "\n";
    }
    return shader;
}

GLuint __link_shaders(GLuint vert, GLuint frag){
    GLuint shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);

    GLint success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(shader, 512, nullptr, info);
        std::cerr << "Program link error: " << info << "\n";
        return 0;
    }

    return shader;
}

texture_t create_texture(void* pixels, int width, int height){
    texture_t tex;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
   
    tex.width = width;
    tex.height = height;

    return tex;
}

renderer_shader_t create_shader(std::string vert, std::string frag){
    GLuint v = __compile_shader(GL_VERTEX_SHADER, vert);
    if(v == 0) return 0;
    GLuint f = __compile_shader(GL_FRAGMENT_SHADER, frag);
    if(f == 0) return 0;
    GLuint s = __link_shaders(v, f);
    glDeleteShader(v);
    glDeleteShader(f);
    return s;
}
renderer_unbatched_object_t create_object(float* vertices, unsigned int count, unsigned int* indices, unsigned int idx_count, renderer_shader_t shader){
    VAO* va = new VAO();
    va->Bind();
    VBO* vb = new VBO(vertices, count*sizeof(float));
    va->LinkAttrib(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    IBO* ib = new IBO(indices, idx_count*sizeof(unsigned int));
    return (uintptr_t)new renderer_object_t{va, ib, (GLuint)shader, idx_count, {}};
}

renderer_unbatched_object_t create_object_textured(float* vertices, unsigned int count, unsigned int* indices, unsigned int idx_count, renderer_shader_t shader, texture_t texture,std::string uniform){
    VAO* va = new VAO();
    va->Bind();
    VBO* vb = new VBO(vertices, count*sizeof(float));
    va->LinkAttrib(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
va->LinkAttrib(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    IBO* ib = new IBO(indices, idx_count*sizeof(unsigned int));
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    GLuint location = glGetUniformLocation(shader, uniform.c_str());
            GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "GL error after getting location: " << error << "\n";
    }
    glUniform1i(location, 0);
     error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "GL error after setting uniform: " << error << "\n";
    }

    return (uintptr_t)new renderer_object_t{va, ib, (GLuint)shader, idx_count, texture};
}

void renderer_draw_object(renderer_unbatched_object_t object){
    renderer_object_t* obj = (renderer_object_t*)object;
    glUseProgram(obj->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj->texture.id);
    obj->vao->Bind();
    glDrawElements(GL_TRIANGLES, obj->num_ind, GL_UNSIGNED_INT, nullptr);
    obj->vao->Unbind();
    glUseProgram(0);
}