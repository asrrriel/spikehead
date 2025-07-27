#pragma once

#include <string>
#include "math/vector.h"
#include "math/matrix.h"

class Shader {
    unsigned int ID;
public:
    Shader(std::string vert, std::string frag);
    ~Shader();
    void SetUniform1f(std::string name, float value);
    void SetUniform1i(std::string name, int value);
    void SetUniform3f(std::string name, Vec3 &value);
    void SetUniform4f(std::string name, Vec4 &value);
    void SetUniform4x4f(std::string name, Mat4 &value);
    void Bind();
    void Unbind();
};