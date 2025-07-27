#pragma once

#include <cstddef>
#include <string>

struct wpdl_result_t {
    bool error;
    size_t x,y,width, height;
};

wpdl_result_t wpdl_parse(const std::string& wpdl,wpdl_result_t first_window,wpdl_result_t last_window,wpdl_result_t screen);