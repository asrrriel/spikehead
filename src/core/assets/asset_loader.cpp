#include "assets.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include "libs/simdjson.h"
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

static struct {
    simdjson::ondemand::document doc;
    simdjson::ondemand::parser parser;
    simdjson::padded_string json_string;
    asset_pack_location_t pack;
} asset_registry;

asset_pack_location_t find_asset_pack(){
    asset_pack_location_t toret;

    //find an assets,json
    std::filesystem::path p = std::filesystem::current_path();

    if (std::filesystem::exists(p / "assets.json")) {
        toret.in_memory = false;
        toret.path = p.string();
    } else if (std::filesystem::exists(p / "assets" / "assets.json")) {
        p = p / "assets";
        toret.in_memory = false;
        toret.path = p.string();
    } else {
        //TODO: packed asset package
    }
    
    return toret;
}

bool load_asset_pack(asset_pack_location_t pack){
    if (pack.in_memory) {
        return false; // TODO
    } else {
        asset_registry.json_string = simdjson::padded_string::load(pack.path + "/assets.json");
    }

    asset_registry.doc = asset_registry.parser.iterate(asset_registry.json_string); 
    if(asset_registry.doc.is_alive() == false){
        std::cerr << "Failed to parse assets.json\n";
        return false;
    }
    asset_registry.pack = pack;
    return true;
}

asset_descriptor_t lookup_asset(std::string intern_id) {
    asset_descriptor_t toret{
        .error = true
    };

    // Try to look up the asset by ID
    auto asset_result = asset_registry.doc[intern_id];
    if (asset_result.error()) {
        std::cerr << "Asset ID '" << intern_id << "' not found in registry.\n";
        return toret; // default-constructed
    }

    auto type = asset_result["type"];
    if (type.error()) {
        std::cerr << "Missing 'type' field for asset '" << intern_id << "'.\n";
        return toret;
    }

    toret.type = static_cast<asset_type_t>(static_cast<uint8_t>(type));

    auto path = asset_result["path-" PLATFORM];
    if (path.error()) {
        // Fallback to generic "path"
        path = asset_result["path"];
        if (path.error()) {
            std::cerr << "Missing platform-specific and fallback 'path' for asset '" << intern_id << "'.\n";
            return toret;
        }
    }

    toret.path = asset_registry.pack.path + "/" + std::string(path);
    toret.error = false;

    return toret;
}

texture_t load_texture(asset_descriptor_t descriptor){
    if (descriptor.error){
        return {};
    }

    //TODO: in memory assets

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(descriptor.path.c_str(), &width, &height, &channels, 0);

    if(!data){
        std::cerr << "Failed to load texture '" << descriptor.path << "'\n";
        return {};
    }

    return create_texture(data, width, height);
}