#pragma once

#include <cstdint>

#define MAX_COMPS 64

#define COMP_TYPE_TRANSFORM_2D 1
#define COMP_TYPE_SQUARE       2
#define COMP_TYPE_MAT_COLOR    3
#define COMP_TYPE_MAT_TEXTURE  4

class Entity {
    void* components[MAX_COMPS];
    uint16_t component_types[MAX_COMPS];
    
public:
    Entity();

    bool add_component(uint16_t type, void* component);
    
    bool has_component(uint16_t type);
    void* get_component(uint16_t type);
    bool remove_component(uint16_t id);
    
};