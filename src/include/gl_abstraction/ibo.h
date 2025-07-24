#pragma once 

class IBO {
    unsigned int ID;
public:
    IBO(unsigned int* indices, unsigned int size);
    ~IBO();
    void Update(unsigned int* indices, unsigned int size);
    void Bind();
    void Unbind();
};