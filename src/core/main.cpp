#include "platform.h"
#include "renderer.h"
#include "math.h"
#include <chrono>
#include <thread>

int main() {
    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    platform_screen_t screen = platform_get_primary_screen(ctx);
    platform_window_t window = platform_create_window(ctx, screen, 800, 600);
    platform_set_title(ctx, window, "Hello " PLATFORM "!");
    platform_show_window(ctx, window);

    if(!renderer_init(ctx, window)){
        exit(69);
    }
    
    while(!platform_should_close(ctx, window)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        auto now = std::chrono::steady_clock::now();
        auto duration = now.time_since_epoch();

        // convert duration to floating seconds
        double seconds = duration_cast<std::chrono::duration<double>>(duration).count();

        // get fractional part between 0 and 1
        float hue = std::fmod(seconds, 3.1415);
        
        float r = fabs(sin(hue));
        float g = fabs(sin(hue + 2.094));
        float b = fabs(sin(hue + 4.188));
        
        renderer_setbgcol(r,g,b);
        
        
        renderer_clear();
        renderer_swap();
    }

    renderer_deinit();
    platform_destroy_window(ctx, window);
    platform_deinit(ctx);

    return 0;
}
