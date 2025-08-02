#include "math/transform.h"
#include <cmath>

Mat4 get_identity(){
    return Mat4((float[]){
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1}); 
}
Mat4 rotate(Vec4 rot) {
    float w = rot[0], x = rot[1], y = rot[2], z = rot[3];

    return Mat4((float[]){
        1 - 2*(y*y + z*z),  2*(x*y - w*z),      2*(x*z + w*y),      0,
        2*(x*y + w*z),      1 - 2*(x*x + z*z),  2*(y*z - w*x),      0,
        2*(x*z - w*y),      2*(y*z + w*x),      1 - 2*(x*x + y*y),  0,
        0,                  0,                  0,                  1
    });
}


Mat4 translate(Vec3 offset) {
    return Mat4((float[]){
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        offset[0], offset[1], offset[2], 1
    });
}

Mat4 scale(Vec3 scale) {
    return Mat4((float[]){
        scale[0], 0, 0, 0,
        0, scale[1], 0, 0,
        0, 0, scale[2], 0,
        0, 0, 0, 1
    });
}

Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
    return Mat4((float[]){
        2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, -2 / (far - near), 0,
        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        -(far + near) / (far - near),
        1
    });
}