#pragma once
#include "platform.h"
#include <GL/gl.h>
#include <vector>
#include "math/vector.h"
#include "gl_abstraction/texture.h"
#include "ecs.h"

struct transform_t {
    Vec3 position;
    Vec3 scale;
    Vec3 rotation;
};

struct color_material_t {
    Vec4 color;
};

struct texture_material_t {
    Texture* texture;
};

platform_gl_context_t renderer_init(platform_context_t context, platform_window_t window);

bool renderer_setbgcol(float r, float g, float b, float a);
bool renderer_clear();

void* renderer_create_color_material(Vec4 color);
void* renderer_create_texture_material(std::string texture);

void* renderer_create_transform(Vec3 position, Vec3 scale, Vec3 rotation);

void renderer_draw(std::vector<Entity>);