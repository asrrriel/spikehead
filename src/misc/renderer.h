#pragma once
#include "platform.h"
#include <GL/gl.h>

typedef uintptr_t renderer_shader_t;
typedef uintptr_t renderer_unbatched_object_t;
struct texture_t {
    GLuint id;
    int width;
    int height;
};

bool renderer_init(platform_context_t context, platform_window_t window);

bool renderer_setbgcol(float r, float g, float b);
bool renderer_clear();

renderer_shader_t create_shader(std::string vert, std::string frag);
texture_t create_texture(void* pixels, int width, int height);

renderer_unbatched_object_t create_object(float* vertices, unsigned int size, unsigned int* indices, unsigned int index_size, renderer_shader_t shader);
renderer_unbatched_object_t create_object_textured(float* vertices, unsigned int count, unsigned int* indices, unsigned int idx_count, renderer_shader_t shader, texture_t texture,std::string uniform);

void renderer_draw_object(renderer_unbatched_object_t object);



void renderer_swap();


void renderer_deinit();