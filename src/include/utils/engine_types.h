#pragma once 

#include <math/vector.h>
#include <math/matrix.h>
#include <cstddef>

typedef struct {
    bool error;
    size_t x,y,width, height;
} sh_rect_t;