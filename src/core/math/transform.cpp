#include "math/transform.h"
#include <cmath>

Mat4 get_identity(){
    return Mat4((float[]){
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1}); 
}
Mat4 rotate(Vec3 rot){
    float sinx = std::sin(rot[0]);
    float cosx = std::cos(rot[0]);
    float siny = std::sin(rot[1]);
    float cosy = std::cos(rot[1]);
    float sinz = std::sin(rot[2]);
    float cosz = std::cos(rot[2]);

    Mat4 rotx((float[]){
        1,0,0,0,
        0,cosx,-sinx,0,
        0,sinx,cosx,0,
        0,0,0,1});
    Mat4 roty((float[]){
        cosy,0,siny,0,
        0,1,0,0,
        -siny,0,cosy,0,
        0,0,0,1});
    Mat4 rotz((float[]){
        cosz,-sinz,0,0,
        sinz,cosz,0,0,
        0,0,1,0,
        0,0,0,1});

    return rotz * roty * rotx;
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