#pragma once
#include "platform.h"
#include <GL/gl.h>
#include <vector>
#include "math/vector.h"
#include "gl_abstraction/texture.h"
#include "ecs.h"

struct color_material_t {
    Vec3f color;
};

struct texture_material_t {
    Texture* texture;
};

bool renderer_init(platform_context_t context, platform_window_t window);

bool renderer_setbgcol(float r, float g, float b);
bool renderer_clear();

void* renderer_create_color_material(float r, float g, float b);
void* renderer_create_texture_material(std::string texture);

void renderer_draw(std::vector<Entity>);

void renderer_swap();

void renderer_deinit();