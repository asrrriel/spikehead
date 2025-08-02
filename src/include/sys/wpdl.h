#pragma once

#include <utils/engine_types.h>
#include <string>

sh_rect_t wpdl_parse(const std::string& wpdl,sh_rect_t first_window,sh_rect_t last_window,sh_rect_t screen);