#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <cstdint>

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

#endif // __WINDOW_H__