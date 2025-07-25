#include "ecs.h"
#include "platform.h"
#include "sys/renderer.h"
#include "sys/assets.h"
#include "math.h"
#include <iostream>
#include <chrono>
#include <thread>

std::vector<Entity> entities;

void resize_callback(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer){
    glViewport(0, 0, width, height);
}

int main() {
    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    #ifdef _WIN32
        std::cout << "[WARNING] Window composition is not supported on Windows" << std::endl;
    #endif

    platform_screen_t screen = platform_get_primary_screen(ctx);
    platform_window_t window = platform_create_window(ctx, screen, 800, 600, true);
    platform_set_title(ctx, window, "Hello " PLATFORM "!");
    platform_show_window(ctx, window);
    platform_register_resize_callback(ctx, window, 0, resize_callback);

    if(!renderer_init(ctx, window)){
        exit(69);
    }

    if(!load_asset_pack(find_asset_pack())){
        exit(42);
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
    
    while(!platform_should_close(ctx, window)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        renderer_setbgcol(0,0,0,0);
        renderer_clear();

        renderer_draw(entities);

        renderer_swap();

        size_t x,y;
        platform_get_position(ctx, window, &x, &y);
        platform_set_position(ctx, window, x+1, y);

    }

    renderer_deinit();
    platform_destroy_window(ctx, window);
    platform_deinit(ctx);

    return 0;
}
