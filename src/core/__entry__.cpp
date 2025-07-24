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

    platform_screen_t screen = platform_get_primary_screen(ctx);
    platform_window_t window = platform_create_window(ctx, screen, 800, 600);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        auto now = std::chrono::steady_clock::now();
        auto duration = now.time_since_epoch();

        double seconds = duration_cast<std::chrono::duration<double>>(duration).count();

        float hue = std::fmod(seconds, 3.1415);

        float r = fabs(sin(hue));
        float g = fabs(sin(hue + 2.094));
        float b = fabs(sin(hue + 4.188));

        renderer_setbgcol(r,g,b);
        renderer_clear();

        renderer_draw(entities);

        renderer_swap();
    }

    renderer_deinit();
    platform_destroy_window(ctx, window);
    platform_deinit(ctx);

    return 0;
}
