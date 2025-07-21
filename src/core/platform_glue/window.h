#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <cstdint>

// Windowing stuff
typedef std::uintptr_t platform_context_t;
typedef std::uintptr_t platform_screen_t;
typedef std::uintptr_t platform_window_t;

platform_context_t platform_init();
void platform_deinit(platform_context_t context);

platform_screen_t platform_get_primary_screen(platform_context_t context);

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height);

bool platform_should_close(platform_context_t context, platform_window_t window);

bool platform_set_title(platform_context_t context, platform_window_t window, const char* title);
void platform_show_window(platform_context_t context, platform_window_t window);
void platform_destroy_window(platform_context_t context, platform_window_t window);

// OpenGL Stuff
typedef std::uintptr_t platform_gl_context_t;  

platform_gl_context_t platform_create_gl_context(platform_context_t context, platform_window_t window);
void platform_make_context_current(platform_gl_context_t gl_context);
void platform_swap_buffers(platform_gl_context_t gl_context);
void platform_destroy_gl_context(platform_gl_context_t gl_context);

#endif // __WINDOW_H__