#pragma once
#include "platform.h"

typedef uintptr_t renderer_shader_t;
typedef uintptr_t renderer_unbatched_object_t;

bool renderer_init(platform_context_t context, platform_window_t window);

bool renderer_setbgcol(float r, float g, float b);
bool renderer_clear();

renderer_shader_t create_shader(std::string vert, std::string frag);
renderer_unbatched_object_t create_object(float* vertices, unsigned int size, unsigned int* indices, unsigned int index_size, renderer_shader_t shader);

void renderer_draw_object(renderer_unbatched_object_t object);

void renderer_swap();


void renderer_deinit();