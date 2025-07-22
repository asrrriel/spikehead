#include "platform.h"
#include "renderer.h"
#include "GL/gl.h"
#include <iostream>

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