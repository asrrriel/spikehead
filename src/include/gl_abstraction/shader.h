#pragma once

#include <string>
#include "math/vector.h"

class Shader {
    unsigned int ID;
public:
    Shader(std::string vert, std::string frag);
    ~Shader();
    void SetUniform1f(std::string name, float value);
    void SetUniform3f(std::string name, Vec3f value);
    void Bind();
    void Unbind();
};