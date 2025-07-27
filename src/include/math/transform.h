#pragma once

#include "vector.h"
#include "matrix.h"

Mat4 get_identity();

Mat4 rotate(Vec3 rot);
Mat4 translate(Vec3 trans);
Mat4 scale(Vec3 scale);