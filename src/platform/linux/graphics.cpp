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
#include <unordered_map>
#include "platform.h"

struct platform_context {
    Display* display;
    Atom wm_delete_window;
    GLXFBConfig fbc;
    XVisualInfo* vi;
    Colormap cmap;

    platform_context(Display* _display, Atom _wm_delete_window, GLXFBConfig _fbc, XVisualInfo* _vi, Colormap _cmap) 
    : display(_display), wm_delete_window(_wm_delete_window), fbc(_fbc), vi(_vi), cmap(_cmap) {}
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
    size_t x,y,width, height;
    bool should_close = false;

    x_window_t(Window _window) : window(_window) {
        x = y = width = height = 0;
    }
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

    Colormap cmap = XCreateColormap(display, RootWindowOfScreen(DefaultScreenOfDisplay(display)),vi->visual, AllocNone);

    platform_context* ctx = new platform_context(display, wm_delete_window,bestFbc, vi,cmap);
    return reinterpret_cast<platform_context_t>(ctx);
}

void platform_deinit(platform_context_t context) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    XCloseDisplay(ctx->display);
    if(ctx->vi) XFree(ctx->vi);
    delete ctx;
}

platform_screen_t platform_get_primary_screen(platform_context_t context) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    return reinterpret_cast<platform_screen_t>(DefaultScreenOfDisplay(ctx->display));
}

screen_size_t platform_get_screen_size(platform_context_t context, platform_screen_t screen){
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    Screen* scr = reinterpret_cast<Screen*>(screen);
    return {0,0,0,static_cast<size_t>(scr->width), static_cast<size_t>(scr->height)};
}

std::unordered_map<Window, x_window_t*> window_map;

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height, bool borderless) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    Screen* scr = reinterpret_cast<Screen*>(screen);

    XSetWindowAttributes swa;
    swa.colormap = ctx->cmap;
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    Window window = XCreateWindow(
        ctx->display, 
        RootWindowOfScreen(scr)
        , 0, 0, width, height, 0,
        ctx->vi->depth,
        InputOutput,
        ctx->vi->visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &swa
    );

    XSelectInput(ctx->display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XSetWMProtocols(ctx->display, window, &ctx->wm_delete_window, 1);

    if(borderless) {
        Atom wm_type_atom = XInternAtom(ctx->display, "_NET_WM_WINDOW_TYPE", False);
        Atom wm_type_value = XInternAtom(ctx->display, "_NET_WM_WINDOW_TYPE_DOCK", False);
        
        XChangeProperty(ctx->display, window, wm_type_atom, XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)&wm_type_value, 1);
    }

    x_window_t* x_window = new x_window_t(window);

    window_map[window] = x_window;

    return reinterpret_cast<platform_window_t>(x_window);
}

bool platform_set_title(platform_context_t context, platform_window_t window, std::string title) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);
    XStoreName(ctx->display, win->window, title.c_str());
    XFlush(ctx->display);
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

bool platform_get_size(platform_context_t context, platform_window_t window, std::size_t* width, std::size_t* height){
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* win = reinterpret_cast<x_window_t*>(window);

    *width = win->width;
    *height = win->height;

    return true;
}


bool platform_should_close(platform_context_t context, platform_window_t window) {
    platform_context* ctx = reinterpret_cast<platform_context*>(context);
    x_window_t* original_win = reinterpret_cast<x_window_t*>(window);

    //process events for all windows
    x_window_t* win = original_win;
    while (XPending(ctx->display)) {
        XEvent event;
        XNextEvent(ctx->display, &event);
        win = window_map[event.xany.window];

        switch(event.type) {
            case ClientMessage:
                if (static_cast<Atom>(event.xclient.data.l[0]) == ctx->wm_delete_window)
                    win->should_close = true;
                break;
            case DestroyNotify:
                win->should_close = true;
                break;
            case ConfigureNotify:
                if(win->resize_callback) {
                    win->resize_callback(window, event.xconfigure.width, event.xconfigure.height, win->resize_private_pointer);
                }
                win->width = event.xconfigure.width;
                win->height = event.xconfigure.height;
                win->x = event.xconfigure.x;
                win->y = event.xconfigure.y;
                break;
            default:
                break;
        }

    }
    return original_win->should_close;
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
    delete win;
}

platform_gl_context_t platform_create_gl_context(platform_context_t context, platform_window_t window, platform_gl_context_t shares_with) {
    platform_context* pctx = reinterpret_cast<platform_context*>(context);
    x_window_t* xwin = reinterpret_cast<x_window_t*>(window);

    glx_context* shared_ctx = reinterpret_cast<glx_context*>(shares_with);


    typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 
        (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    if (!glXCreateContextAttribsARB) {
        std::cerr << "glXCreateContextAttribsARB not available.\n";
        return 0;
    }

    int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    GLXContext share = shared_ctx ? shared_ctx->gl_context : None;
    GLXContext glc = glXCreateContextAttribsARB(pctx->display,pctx->fbc, share, True, attribs);
    
    if (glc == NULL) {
        std::cerr << "Failed to create GLX context\n";
        return 0;
    }

    glx_context* ctx = new glx_context(glc, pctx->display, xwin->window);
    return reinterpret_cast<platform_gl_context_t>(ctx);
}

platform_gl_context_t current_context = 0;

void platform_make_context_current(platform_gl_context_t gl_context) {
    glx_context* ctx = reinterpret_cast<glx_context*>(gl_context);
    current_context = gl_context;
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
    delete ctx;
}

platform_gl_context_t platform_get_current_gl_context(){
    return current_context;
}