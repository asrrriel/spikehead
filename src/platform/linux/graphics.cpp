#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <cstddef>
#include <iostream>
#include <cstring>
#include "window.h"

struct platform_context {
    Display* display;
    Atom wm_delete_window;
};

struct glx_context {
    GLXContext gl_context;
    ::Display* x_display;
    ::Window window;
};

platform_context_t platform_init() {
    platform_context* ctx = new platform_context;
    ctx->display = XOpenDisplay(nullptr);
    if (!ctx->display) {
        std::cerr << "Cannot open X11 display\n";
        delete ctx;
        return 0;
    }
    ctx->wm_delete_window = XInternAtom(ctx->display, "WM_DELETE_WINDOW", False);
    return reinterpret_cast<platform_context_t>(ctx);
}

void platform_deinit(platform_context_t context) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    XCloseDisplay(ctx->display);
    delete ctx;
}

platform_screen_t platform_get_primary_screen(platform_context_t context) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    return reinterpret_cast<platform_screen_t>(DefaultScreenOfDisplay(ctx->display));
}

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    Screen* scr = reinterpret_cast<Screen*>(screen);

    Window window = XCreateSimpleWindow(ctx->display, RootWindowOfScreen(scr), 0, 0, width, height, 0,
                                        BlackPixelOfScreen(scr), BlackPixelOfScreen(scr));

    XSelectInput(ctx->display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XSetWMProtocols(ctx->display, window, &ctx->wm_delete_window, 1);
    return reinterpret_cast<platform_window_t>(window);
}

bool platform_set_title(platform_context_t context, platform_window_t window, const char* title) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    ::Window win = reinterpret_cast<::Window>(window);
    XStoreName(ctx->display, win, title);
    return true;
}

bool platform_should_close(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    ::Window win = reinterpret_cast<::Window>(window);

    while (XPending(ctx->display)) {
        XEvent event;
        XNextEvent(ctx->display, &event);

        if (event.type == ClientMessage && static_cast<Atom>(event.xclient.data.l[0]) == ctx->wm_delete_window)
            return true;

    }
    return false;
}

void platform_show_window(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    XMapWindow(ctx->display, reinterpret_cast<::Window>(window));
    XFlush(ctx->display);
}

void platform_destroy_window(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    XDestroyWindow(ctx->display, reinterpret_cast<::Window>(window));
}

platform_gl_context_t platform_create_gl_context(platform_context_t context, platform_window_t window) {
    platform_context* pctx = reinterpret_cast<platform_context*>(context);
    Window win = reinterpret_cast<Window>(window);

    GLint attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo *vi = glXChooseVisual(pctx->display, 0, attribs);
    GLXContext glc = glXCreateContext(pctx->display, vi, NULL, GL_TRUE);

    if (glc == NULL) {
        std::cerr << "Failed to create GLX context\n";
        return 0;
    }

    glx_context* ctx = new glx_context;
    ctx->gl_context = glc;
    ctx->x_display = pctx->display;
    ctx->window = win;
    return reinterpret_cast<platform_gl_context_t>(ctx);
}

void platform_make_context_current(platform_gl_context_t gl_context) {
    glx_context* ctx = reinterpret_cast<glx_context*>(gl_context);
    if(!glXMakeCurrent(ctx->x_display, ctx->window, ctx->gl_context)) {
        std::cerr << "Failed to make GLX context current\n";
    }
}

void platform_swap_buffers(platform_gl_context_t gl_context) {
    glx_context* ctx = reinterpret_cast<glx_context*>(gl_context);
    glXSwapBuffers(ctx->x_display, ctx->window);
}

void platform_destroy_gl_context(platform_gl_context_t gl_context) {
    glx_context* ctx = reinterpret_cast<glx_context*>(gl_context);
    glXDestroyContext(ctx->x_display, ctx->gl_context);
}
