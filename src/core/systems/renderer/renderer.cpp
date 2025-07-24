#include "platform.h"
#include "glad/include/glad/glad.h"
#include "GL/gl.h"
#include "sys/renderer.h"
#include "ecs.h"
#include "gl_abstraction/vbo.h"
#include "gl_abstraction/vao.h"
#include "gl_abstraction/ibo.h"
#include "gl_abstraction/shader.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sys/types.h>
#include <vector>

platform_gl_context_t gl_context;

extern Shader* color_shader;
extern VAO   * square_vao;
extern IBO   * square_ibo;

void init_defaults();

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

    const char* vendor = (const char*)glGetString(GL_VERSION);
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout << "OpenGL version: " <<  major << '.' << minor << '\n';
    std::cout << "OpenGL vendor: " << vendor << '\n'; 

    __renderer_print_errors();
    init_defaults();

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

void __render_material(Entity e, VAO* vao, GLuint index_count){
    if(e.has_component(COMP_TYPE_MAT_COLOR)){
        color_shader->Bind();
        color_shader->SetUniform3f("color", Vec3f(1.0f));
    } else {
        std::cout << "No material\n";
        return;
    }

    vao->Bind();
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    __renderer_print_errors();
    vao->Unbind();
    color_shader->Unbind();
}

void renderer_draw(std::vector<Entity> entities){
    for (auto e : entities) {
        if(e.has_component(COMP_TYPE_SQUARE)){
            __render_material(e, square_vao, 6);
        }
    } 
}