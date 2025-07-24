#include "ecs.h"

Entity::Entity() {
    for (int i = 0; i < MAX_COMPS; i++) {
        components[i] = nullptr;
        component_types[i] = 0;
    }
}

bool Entity::has_component(uint16_t type) {
    for (int i = 0; i < MAX_COMPS; i++) {
        if (component_types[i] == type) {
            return true;
        }
    }
    return false;
}

bool Entity::add_component(uint16_t type, void* component) {
    for (int i = 0; i < MAX_COMPS; i++) {
        if (component_types[i] == type) {
            return false; // only one component per type
        }
        if (component_types[i] == 0) {
            component_types[i] = type;
            components[i] = component;
            return true;
        }
    }
    return false;
}

void* Entity::get_component(uint16_t type){
    for (int i = 0; i < MAX_COMPS; i++) {
        if (component_types[i] == type) {
            return components[i];
        }
    }
    return nullptr;
}

bool Entity::remove_component(uint16_t type) {
    for (int i = 0; i < MAX_COMPS; i++) {
        if (component_types[i] == type) {
            component_types[i] = 0;
            components[i] = nullptr;
            return true;
        }
    }
    return false;
}