#include "window.h"
#include <windef.h>
#include <windows.h>
#include <string>
#include <iostream>

struct Win32Context {
    HINSTANCE hInstance;
    ATOM classAtom;

    Win32Context(HINSTANCE _hInstance, ATOM _classAtom)
        : hInstance(_hInstance), classAtom(_classAtom) {}
};

struct Win32Window {
    HWND hwnd;
    bool shouldClose;

    Win32Window(HWND hwnd_, bool shouldClose_)
        : hwnd(hwnd_), shouldClose(shouldClose_) {}
};

struct Win32GlContext {
    HDC dc;
    HGLRC glc;

    Win32GlContext(HDC _dc, HGLRC _glc)
        : dc(_dc), glc(_glc) {}
};

LRESULT CALLBACK PlatformWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32Window* win = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_CLOSE:
            if (win) win->shouldClose = true;
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

platform_context_t platform_init() {
    Win32Context* ctx = new Win32Context(GetModuleHandle(NULL), 0);

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = ctx->hInstance;
    wc.lpfnWndProc = PlatformWndProc;
    wc.lpszClassName = "MyPlatformWindowClass";

    ctx->classAtom = RegisterClassEx(&wc);
    if (!ctx->classAtom) {
        delete ctx;
        return 0;
    }

    return reinterpret_cast<platform_context_t>(ctx);
}

void platform_deinit(platform_context_t context) {
    Win32Context* ctx = reinterpret_cast<Win32Context*>(context);
    if (!ctx) return;

    UnregisterClass(MAKEINTATOM(ctx->classAtom), ctx->hInstance);
    delete ctx;
}

platform_screen_t platform_get_primary_screen(platform_context_t context) {
    return 1;
}

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height) {
    Win32Context* ctx = reinterpret_cast<Win32Context*>(context);
    if (!ctx) return 0;

    HWND hwnd = CreateWindowEx(
        0,
        MAKEINTATOM(ctx->classAtom),
        "Untitled Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        (int)width, (int)height,
        NULL,
        NULL,
        ctx->hInstance,
        NULL
    );
    if (!hwnd) return 0;

    
    Win32Window* win = new Win32Window(hwnd, false);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(win));
    return reinterpret_cast<platform_window_t>(win);
}

bool platform_should_close(platform_context_t context, platform_window_t window) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if (!win) return true; // terminate the main loop if the window is nonexistant
    return win->shouldClose;
}

bool platform_set_title(platform_context_t context, platform_window_t window, std::string title) {
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if (!win || !win->hwnd) return false;

    return SetWindowTextA(win->hwnd, title.c_str()) != 0;
}

void platform_show_window(platform_context_t context, platform_window_t window) {
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if (!win || !win->hwnd) return;

    ShowWindow(win->hwnd, SW_SHOW);
    UpdateWindow(win->hwnd);
}

void platform_destroy_window(platform_context_t context, platform_window_t window) {
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if (!win) return;

    DestroyWindow(win->hwnd);
    delete win;
}

platform_gl_context_t platform_create_gl_context(platform_context_t context, platform_window_t window){
    Win32Context* ctx = reinterpret_cast<Win32Context*>(context);
    Win32Window* win = reinterpret_cast<Win32Window*>(window);

    HDC dc = GetDC(win->hwnd);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(dc, &pfd);
    if(!SetPixelFormat(dc, pixelFormat, &pfd)){
        std::cerr << "Failed to set pixel format\n";
        return 0;
    }

    HGLRC glc = wglCreateContext(dc);
    if(!glc) {
        std::cerr << "Failed to create GL context\n";
        ReleaseDC(win->hwnd, dc);
        return 0;
    }

    Win32GlContext* glctx = new Win32GlContext(dc,glc);
    return reinterpret_cast<platform_gl_context_t>(glctx);
}
void platform_make_context_current(platform_gl_context_t gl_context){
    Win32GlContext* ctx = reinterpret_cast<Win32GlContext*>(gl_context);
    if(!wglMakeCurrent(ctx->dc, ctx->glc)){
        std::cerr << "Failed to make GL context current\n";
    }
}
void platform_swap_buffers(platform_gl_context_t gl_context){
    Win32GlContext* ctx = reinterpret_cast<Win32GlContext*>(gl_context);
    if(!SwapBuffers(ctx->dc)){
        std::cerr << "Failed to swap buffers\n";
    }
}
void platform_destroy_gl_context(platform_gl_context_t gl_context){
    Win32GlContext* ctx = reinterpret_cast<Win32GlContext*>(gl_context);
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(ctx->glc);
    DeleteDC(ctx->dc);
    delete ctx;
}