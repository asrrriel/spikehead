#include "platform.h"
#include "renderer.h"

int main() {
    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    platform_screen_t screen = platform_get_primary_screen(ctx);
    platform_window_t window = platform_create_window(ctx, screen, 800, 600);
    platform_set_title(ctx, window, "Hello " PLATFORM "!");
    platform_show_window(ctx, window);

    renderer_init(ctx, window);
    renderer_setbgcol(1.0f, 0.0f, 0.0f);
    
    while(!platform_should_close(ctx, window)) {
        renderer_clear();
        renderer_swap();
    }

    renderer_deinit();
    platform_destroy_window(ctx, window);
    platform_deinit(ctx);

    return 0;
}
