#include "ecs.h"
#include "platform.h"
#include "sys/renderer.h"
#include "sys/assets.h"
#include "math.h"
#include <iostream>
#include <chrono>
#include <thread>

struct engine_window_t {
    platform_window_t window;
    platform_gl_context_t gl_context;

    engine_window_t(platform_window_t _window, platform_gl_context_t _gl_context){
        window = _window;
        gl_context = _gl_context;
    }
};

std::vector<engine_window_t> windows;
std::vector<Entity> entities;

void resize_callback(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer){
    glViewport(0, 0, width, height);
}

int main() {
    asset_pack_location_t pack = find_asset_pack();

    if(pack.error){
        std::cerr << "[FATAL] Failed to find asset pack" << std::endl;
        exit(69);
    }

    if(!load_asset_pack(pack)){
        exit(42);
    }

    project_manifest_t manifest = get_project_manifest();

    if(manifest.error){
        std::cerr << "[FATAL] Failed to load project manifest" << std::endl;
        exit(69);
    }

    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    #ifdef _WIN32
        std::cout << "[WARNING] Window composition is not supported on Windows" << std::endl;
    #endif

    platform_screen_t screen = platform_get_primary_screen(ctx);

    for(auto window : manifest.windows){
        platform_window_t p_window = platform_create_window(ctx, screen, window.width, window.height, false);
        platform_set_title(ctx, p_window, window.name);
        platform_show_window(ctx, p_window);
        platform_register_resize_callback(ctx, p_window, 0, resize_callback);
    
        platform_gl_context_t gl_context = renderer_init(ctx, p_window);

        windows.push_back(engine_window_t(p_window, gl_context));
    }

    Entity e; 

    if(!e.add_component(COMP_TYPE_SQUARE, NULL)){
        exit(69);
    }

    void* mat = renderer_create_texture_material("test_img");

    if(!e.add_component(COMP_TYPE_MAT_TEXTURE, mat)){
        exit(69);
    }

    entities.push_back(e);

    while(windows.size() != 0){
        for (size_t i = 0; i < windows.size(); ){
            platform_make_context_current(windows[i].gl_context);
            if(platform_should_close(ctx, windows[i].window)){
                platform_destroy_gl_context(windows[i].gl_context);
                platform_destroy_window(ctx, windows[i].window);
                windows.erase(windows.begin() + i);
                continue;
            }
            renderer_setbgcol(0,0,0,0);
            renderer_clear();
            renderer_draw(entities);
            platform_swap_buffers(windows[i].gl_context);
            ++i;
        }
    }

    platform_deinit(ctx);

    return 0;
}
