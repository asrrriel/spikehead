#pragma once

#include <filesystem>
#include <string>
#include "renderer.h"

struct asset_pack_location_t {
    bool error;
    bool in_memory;
    union{
        std::filesystem::path path;
        void* data; // TODO: libzip stuff
    };


    asset_pack_location_t() : error(true), in_memory(false) {
        new (&path) std::filesystem::path();
    }

    // Destructor: destroy active member if needed
    ~asset_pack_location_t() {
        if (!in_memory) {
            path.~path();
        }
    }

    // Copy constructor
    asset_pack_location_t(const asset_pack_location_t& other) : in_memory(other.in_memory) {
        if (in_memory) {
            data = other.data;
        } else {
            new (&path) std::filesystem::path(other.path);
        }
    }

    // Copy assignment
    asset_pack_location_t& operator=(const asset_pack_location_t& other) {
        if (this == &other) return *this;

        if (!in_memory) {
            path.~path();
        }

        in_memory = other.in_memory;

        if (in_memory) {
            data = other.data;
        } else {
            new (&path) std::filesystem::path(other.path);
        }

        return *this;
    }

    // Move constructor
    asset_pack_location_t(asset_pack_location_t&& other) noexcept : in_memory(other.in_memory) {
        if (in_memory) {
            data = other.data;
        } else {
            new (&path) std::filesystem::path(std::move(other.path));
        }
    }

    // Move assignment
    asset_pack_location_t& operator=(asset_pack_location_t&& other) noexcept {
        if (this == &other) return *this;

        if (!in_memory) {
            path.~path();
        }

        in_memory = other.in_memory;

        if (in_memory) {
            data = other.data;
        } else {
            new (&path) std::filesystem::path(std::move(other.path));
        }

        return *this;
    }

};

enum asset_type_t {
    ASSET_TYPE_IMAGE,
    ASSET_TYPE_SHADER,
    ASSET_TYPE_FONT,
    ASSET_TYPE_NATIVE,
    ASSET_TYPE_UNKNOWN
};

struct asset_descriptor_t {
    bool error;
    asset_type_t type;
    std::string path;
};

struct project_window_t {
    int width;
    int height;
    std::string name;
};

struct project_info_t {
    std::string name;
    std::string author;
    std::string version;
};

struct project_manifest_t {
    bool error;
    project_info_t info;
    std::vector<project_window_t> windows;

    project_manifest_t() : error(true) {}
};

asset_pack_location_t find_asset_pack();
bool load_asset_pack(asset_pack_location_t pack);
project_manifest_t get_project_manifest();
asset_descriptor_t lookup_asset(std::string intern_id);