#include "glad/include/glad/glad.h"
#include "ecs.h"
#include "platform.h"
#include "sys/renderer.h"
#include "sys/assets.h"
#include "math.h"
#include <GL/gl.h>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <thread>

struct engine_window_t {
    platform_window_t window;
    platform_gl_context_t gl_context;
    size_t width = 0, height = 0;

    engine_window_t(platform_window_t _window, platform_gl_context_t _gl_context, size_t _width, size_t _height){
        window = _window;
        gl_context = _gl_context;
        width = _width;
        height = _height;
    }
};

std::vector<engine_window_t> windows;
std::vector<Entity> entities;

void resize_callback(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer){
    windows[private_pointer].width = width;
    windows[private_pointer].height = height;
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

    std::cout << manifest.info.name << "(" << manifest.info.version << ") by " << manifest.info.author << std::endl;


    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    #ifdef _WIN32
        std::cout << "[WARNING] Window composition is not supported on Windows" << std::endl;
    #endif

    platform_screen_t screen = platform_get_primary_screen(ctx);

    for(size_t  i = 0; i < manifest.windows.size(); i++){
        platform_window_t p_window = platform_create_window(ctx, screen, manifest.windows[i].width, manifest.windows[i].height, false);
        platform_set_title(ctx, p_window, manifest.windows[i].name);
        platform_show_window(ctx, p_window);
        platform_register_resize_callback(ctx, p_window, i, resize_callback);
    
        platform_gl_context_t gl_context = renderer_init(ctx, p_window);

        windows.push_back(engine_window_t(p_window, gl_context, manifest.windows[i].width, manifest.windows[i].height));
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
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windows[i].width, windows[i].height);
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
