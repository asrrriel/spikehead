#pragma once

class VBO {
    unsigned int ID;
public:
    VBO(float* vertices, unsigned int size);
    ~VBO();
    void Update(float* vertices, unsigned int size);
    void Bind();
    void Unbind();
};