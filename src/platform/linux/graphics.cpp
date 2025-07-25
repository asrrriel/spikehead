#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <X11/Xutil.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include "platform.h"

struct platform_context {
    Display* display;
    Atom wm_delete_window;
    GLXFBConfig fbc;

    platform_context(Display* _display, Atom _wm_delete_window, GLXFBConfig _fbc) 
    : display(_display), wm_delete_window(_wm_delete_window), fbc(_fbc) {}
};

struct glx_context {
    GLXContext gl_context;
    ::Display* x_display;
    ::Window window;

    glx_context(GLXContext _gl_context, ::Display* _x_display, ::Window _window) 
    : gl_context(_gl_context), x_display(_x_display), window(_window) {}
};

struct x_window_t {
    Window window;
    void (*resize_callback)(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer);
    uintptr_t resize_private_pointer;
    size_t x,y;

    x_window_t(Window _window) : window(_window) {}
};

platform_context_t platform_init() {
    ::Display *display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Cannot open X11 display\n";
        return 0;
    }

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    int attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), attribs, &fbcount);
    GLXFBConfig bestFbc = nullptr;
    XVisualInfo* vi = nullptr;

    for (int i = 0; i < fbcount; ++i) {
        XVisualInfo* candidateVi = glXGetVisualFromFBConfig(display, fbc[i]);
        if (!candidateVi)
            continue;

        int depth = candidateVi->depth;
        int alphaBits;
        glXGetFBConfigAttrib(display, fbc[i], GLX_ALPHA_SIZE, &alphaBits);

        if (depth == 32 && alphaBits >= 8) {
            bestFbc = fbc[i];
            vi = candidateVi;
            break;
        }

        XFree(candidateVi);
    }

    if (!bestFbc || !vi) {
        std::cerr << "[WARNING] No suitable framebuffer config with alpha and 32-bit depth found(No Window Transparency)\n";
    }

    XFree(fbc);

    platform_context* ctx = new platform_context(display, wm_delete_window,bestFbc);
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

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height, bool borderless) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    Screen* scr = reinterpret_cast<Screen*>(screen);

    XVisualInfo* vi = glXGetVisualFromFBConfig(ctx->display, ctx->fbc);

    Colormap cmap = XCreateColormap(ctx->display, RootWindowOfScreen(scr), vi->visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    Window window = XCreateWindow(
        ctx->display, 
        RootWindowOfScreen(scr)
        , 0, 0, width, height, 0,
        vi->depth,
        InputOutput,
        vi->visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &swa
    );

    XSelectInput(ctx->display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XSetWMProtocols(ctx->display, window, &ctx->wm_delete_window, 1);

    if(borderless) {
        Atom wm_type_atom = XInternAtom(ctx->display, "_NET_WM_WINDOW_TYPE", False);
        Atom wm_type_value = XInternAtom(ctx->display, "_NET_WM_WINDOW_TYPE_DOCK", False); // or UTILITY
        
        XChangeProperty(ctx->display, window, wm_type_atom, XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)&wm_type_value, 1);
    }

    x_window_t* x_window = new x_window_t(window);

    return reinterpret_cast<platform_window_t>(x_window);
}

bool platform_set_title(platform_context_t context, platform_window_t window, std::string title) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);
    XStoreName(ctx->display, win->window, title.c_str());
    return true;
}

bool platform_set_position(platform_context_t context, platform_window_t window, std::size_t x, std::size_t y){
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);

    
    return XMoveWindow(ctx->display, win->window, x, y);
}
bool platform_set_size(platform_context_t context, platform_window_t window, std::size_t width, std::size_t height){
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);

    return XResizeWindow(ctx->display, win->window, width, height);
}
bool platform_get_position(platform_context_t context, platform_window_t window, std::size_t* x, std::size_t* y){
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);

    *x = win->x;
    *y = win->y;

    return true;
}

bool platform_should_close(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);

    bool toret = false;

    while (XPending(ctx->display)) {
        XEvent event;
        XNextEvent(ctx->display, &event);

        switch(event.type) {
            case ClientMessage:
                if (static_cast<Atom>(event.xclient.data.l[0]) == ctx->wm_delete_window)
                    toret = true;
                break;
            case DestroyNotify:
                toret = true;
            case ConfigureNotify:
                if(win->resize_callback) {
                    win->resize_callback(window, event.xconfigure.width, event.xconfigure.height, win->resize_private_pointer);
                }
                win->x = event.xconfigure.x;
                win->y = event.xconfigure.y;
                break;
            default:
                break;
        }

    }
    return toret;
}

void platform_show_window(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);
    XMapWindow(ctx->display, win->window);
    XFlush(ctx->display);
}

void platform_register_resize_callback(platform_context_t context, platform_window_t window,uintptr_t private_pointer, void (*callback)(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer)){
    x_window_t* win = reinterpret_cast<x_window_t*>(window);
    win->resize_callback = callback;
    win->resize_private_pointer = private_pointer;
}

void platform_destroy_window(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);
    XDestroyWindow(ctx->display, win->window);
}

platform_gl_context_t platform_create_gl_context(platform_context_t context, platform_window_t window) {
    platform_context* pctx = reinterpret_cast<platform_context*>(context);
    x_window_t* xwin = reinterpret_cast<x_window_t*>(window);

    typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 
        (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    if (!glXCreateContextAttribsARB) {
        std::cerr << "glXCreateContextAttribsARB not available.\n";
        exit(1);
    }

    int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    GLXContext glc = glXCreateContextAttribsARB(pctx->display,pctx->fbc, 0, True, attribs);

    if (glc == NULL) {
        std::cerr << "Failed to create GLX context\n";
        return 0;
    }

    glx_context* ctx = new glx_context(glc, pctx->display, xwin->window);
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