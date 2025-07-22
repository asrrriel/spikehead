#pragma once
#include "platform.h"

bool renderer_init(platform_context_t context, platform_window_t window);

bool renderer_setbgcol(float r, float g, float b);
bool renderer_clear();

void renderer_swap();

void renderer_deinit();