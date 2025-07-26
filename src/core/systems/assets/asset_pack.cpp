#include "sys/assets.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include "simdjson/simdjson.h"

#define STB_IMAGE_IMPLEMENTATION
#include "nothings/stb_image.h"

static struct {
    simdjson::ondemand::parser project_parser;
    simdjson::ondemand::parser assets_parser;
    simdjson::padded_string project_json;
    simdjson::padded_string assets_json;
    simdjson::ondemand::document project_doc;
    simdjson::ondemand::document assets_doc;
    asset_pack_location_t pack;
} asset_registry;

asset_pack_location_t find_asset_pack(){
    asset_pack_location_t toret;

    std::filesystem::path p = std::filesystem::current_path();

    if (std::filesystem::exists(p / "project.json")) {
        toret.in_memory = false;
        toret.path = p;
        toret.error = false;
        return toret;
    } else if (std::filesystem::exists(p / "assets" / "project.json")) {
        p = p / "assets";
        toret.in_memory = false;
        toret.path = p;
        toret.error = false;
        return toret;
    } else {
        std::cerr << "In-memory asset packs unimplemented\n";
        return toret;
    }

    toret.error = true;
    return toret;
}
bool load_asset_pack(asset_pack_location_t pack){
    if (pack.in_memory) {
        return false; // TODO
    } else {
        asset_registry.project_json = simdjson::padded_string::load((pack.path / "project.json").string());
        asset_registry.assets_json = simdjson::padded_string::load((pack.path / "assets.json").string());
    }

    asset_registry.project_doc = asset_registry.project_parser.iterate(asset_registry.project_json); 
    asset_registry.assets_doc = asset_registry.assets_parser.iterate(asset_registry.assets_json); 
    if(asset_registry.project_doc.is_alive() == false){
        std::cerr << "Failed to parse project.json\n";
        return false;
    }
    if(asset_registry.assets_doc.is_alive() == false){
        std::cerr << "Failed to parse assets.json\n";
        return false;
    }
    asset_registry.pack = pack;
    return true;
}

project_manifest_t get_project_manifest(){
    project_manifest_t toret;

    //project info
    auto info_result = asset_registry.project_doc["project-info"];
    if (info_result.error()) {
        std::cerr << "[FATAL] project-info not found.\n";
        return toret; // default-constructed
    }

    auto name = info_result["project-name"];
    auto author = info_result["project-author"];
    auto version = info_result["project-version"];

    if(name.error() || author.error() || version.error()){
        std::cerr << "[FATAL] project-info is missing fields, should have: project-name, project-author, project-version .\n";
        return toret;
    }

    toret.info.name = std::string(name);
    toret.info.author = std::string(author);
    toret.info.version = std::string(version);

    //initial windows
    auto windows_result = asset_registry.project_doc["initial-windows"];
    if (windows_result.error()) {
        std::cerr << "[FATAL] initial-windows not found.\n";
        return toret; // default-constructed
    }

    for(auto window : windows_result){
        project_window_t toret_window;
        auto name = window["name"];
        auto width = window["width"];
        auto height = window["height"];
        if(name.error() || width.error() || height.error()){
            std::cerr << "[FATAL] initial-windows is missing fields, should have: name, width, height .\n";
            return toret;
        }
        toret_window.name = std::string(name);
        toret_window.width = static_cast<uint32_t>(width);
        toret_window.height = static_cast<uint32_t>(height);
        toret.windows.push_back(toret_window);
    }

    toret.error = false;
    return toret;
}

asset_descriptor_t lookup_asset(std::string intern_id) {
    asset_descriptor_t toret{
        .error = true
    };

    // Try to look up the asset by ID
    auto asset_result = asset_registry.assets_doc[intern_id];
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

    toret.path = (asset_registry.pack.path / std::string(path)).string();
    toret.error = false;

    return toret;
}