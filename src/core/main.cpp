#include "platform_glue/window.h"
#include "window.h"
#include "GL/gl.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    platform_screen_t screen = platform_get_primary_screen(ctx);
    platform_window_t window = platform_create_window(ctx, screen, 800, 600);
    platform_set_title(ctx, window, "Hello " PLATFORM "!");
    platform_show_window(ctx, window);

    platform_gl_context_t gl_context = platform_create_gl_context(ctx, window);
    platform_make_context_current(gl_context);

    const char* version = (const char*)glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << '\n';
    
    while(!platform_should_close(ctx, window)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // bright red
        glClear(GL_COLOR_BUFFER_BIT);
        platform_swap_buffers(gl_context);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "GL error: " << err << "\n";
        }

    }

    platform_destroy_gl_context(gl_context);
    platform_destroy_window(ctx, window);
    platform_deinit(ctx);

    return 0;
}
