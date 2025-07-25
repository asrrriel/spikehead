#include "platform.h"
#include <windef.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/wglext.h>
#include <string>
#include <iostream>
#include <wingdi.h>
#include <winnt.h>

struct Win32Context {
    HINSTANCE hInstance;
    ATOM classAtom;

    Win32Context(HINSTANCE _hInstance, ATOM _classAtom)
        : hInstance(_hInstance), classAtom(_classAtom) {}
};

struct Win32Window {
    HWND hwnd;
    bool shouldClose;
    void (*resize_callback)(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer);
    uintptr_t resize_private_pointer;
    size_t x,y,width, height;

    Win32Window(HWND hwnd_, bool shouldClose_, size_t width_, size_t height_)
        : hwnd(hwnd_), shouldClose(shouldClose_), width(width_), height(height_) {
            x = y = 0;
        }
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
        case WM_SIZE:
            if (win && win->resize_callback) {
                win->resize_callback(reinterpret_cast<platform_window_t>(win), LOWORD(lParam), HIWORD(lParam), win->resize_private_pointer);
            }
            win->width = LOWORD(lParam);
            win->height = HIWORD(lParam);
            return 0;
        case WM_MOVE:
            win->x = LOWORD(lParam);
            win->y = HIWORD(lParam);
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

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height, bool borderless) {
    Win32Context* ctx = reinterpret_cast<Win32Context*>(context);
    if (!ctx) return 0;

    RECT rect = { 0, 0, (int)width, (int)height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    int new_width = rect.right - rect.left;
    int new_height = rect.bottom - rect.top;

    HWND hwnd = CreateWindowEx(
        0,
        MAKEINTATOM(ctx->classAtom),
        "Untitled Window",
        (borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW),
        CW_USEDEFAULT, CW_USEDEFAULT,
        new_width, new_height,
        NULL,
        NULL,
        ctx->hInstance,
        NULL
    );
    if (!hwnd) return 0;
    
    Win32Window* win = new Win32Window(hwnd, false, width, height);
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

bool platform_set_position(platform_context_t context, platform_window_t window, std::size_t x, std::size_t y){
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    return SetWindowPos(win->hwnd, NULL, x, y, win->width, win->height, SWP_NOZORDER | SWP_NOACTIVATE);
}
bool platform_set_size(platform_context_t context, platform_window_t window, std::size_t width, std::size_t height){
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    return SetWindowPos(win->hwnd, NULL, win->x, win->y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}
bool platform_get_position(platform_context_t context, platform_window_t window, std::size_t* x, std::size_t* y){
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if(!win || !win->hwnd) return false;
    *x = win->x;
    *y = win->y;

    return true;
}

bool platform_get_size(platform_context_t context, platform_window_t window, std::size_t* width, std::size_t* height){
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if(!win || !win->hwnd) return false;
    *width = win->width;
    *height = win->height;

    return true;
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

void platform_register_resize_callback(platform_context_t context, platform_window_t window,uintptr_t private_pointer, void (*callback)(platform_window_t window, std::size_t width, std::size_t height, uintptr_t private_pointer)){
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    win->resize_callback = callback;
    win->resize_private_pointer = private_pointer;
}

platform_gl_context_t platform_create_gl_context(platform_context_t context, platform_window_t window, platform_gl_context_t shares_with) {
    Win32Context* ctx = reinterpret_cast<Win32Context*>(context);
    Win32Window* win = reinterpret_cast<Win32Window*>(window);

    Win32GlContext* sharedCtx = reinterpret_cast<Win32GlContext*>(shares_with);

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

    //FUCK YOU MICROSOFT
    HGLRC tglc = wglCreateContext(dc);
    if(!tglc){
        std::cerr << "Failed to create temporary GL context\n";
        return 0;
    }

    if(!wglMakeCurrent(dc, tglc)){
        std::cerr << "Failed to make temporary GL context current\n";
        wglDeleteContext(tglc);
        return 0;
    }

    typedef HGLRC (*wglCreateContextAttribsARBProc)(HDC, HGLRC, const int*);

    wglCreateContextAttribsARBProc wglCreateContextAttribsARB = 
    (wglCreateContextAttribsARBProc)wglGetProcAddress((const LPCSTR)"wglCreateContextAttribsARB");

    if(!wglCreateContextAttribsARB){
        std::cerr << "Failed to load wglCreateContextAttribsARB\n";
        return 0;
    }

    wglMakeCurrent(nullptr, nullptr); // cant let our temorary context leak out
    wglDeleteContext(tglc);

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC glc = wglCreateContextAttribsARB(dc, 0, attribs);
    if(!glc) {
        std::cerr << "Failed to create GL context\n";
        ReleaseDC(win->hwnd, dc);
        return 0;
    }
    if(shares_with != 0){
        if (!wglShareLists(sharedCtx->glc, glc)) {
            std::cerr << "[FATAL] Failed to share GL contexts\n";
            wglDeleteContext(glc);
            ReleaseDC(win->hwnd, dc);
            return 0;
        }
    }

    Win32GlContext* glctx = new Win32GlContext(dc,glc);
    return reinterpret_cast<platform_gl_context_t>(glctx);
}

platform_gl_context_t current_context;

void platform_make_context_current(platform_gl_context_t gl_context){
    Win32GlContext* ctx = reinterpret_cast<Win32GlContext*>(gl_context);

    current_context = gl_context;

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
    ReleaseDC(WindowFromDC(ctx->dc), ctx->dc);
    delete ctx;
}

platform_gl_context_t platform_get_current_gl_context(){
    return current_context;
}