#pragma once
#include "platform.h"
#include <GL/gl.h>
#include <vector>
#include "ecs.h"

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

void renderer_draw(std::vector<Entity>);

void renderer_swap();

void renderer_deinit();