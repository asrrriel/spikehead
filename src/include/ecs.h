#pragma once

#include <cstdint>

#define MAX_COMPS 64

#define COMP_TYPE_TRANSFORM 0
#define COMP_TYPE_SQUARE    1
#define COMP_TYPE_MAT_COLOR 1

class Entity {
    void* components[MAX_COMPS];
    uint16_t component_types[MAX_COMPS];
    
public:
    Entity();

    bool add_component(uint16_t type, void* component);
    
    bool has_component(uint16_t type);
    bool get_component(uint16_t type, void** component);
    bool remove_component(uint16_t id);
    
};