// window_win32.cpp

#include "window.h"
#include <windows.h>
#include <string>

// To hold Win32 context info
struct Win32Context {
    HINSTANCE hInstance;
    ATOM classAtom;
};

struct Win32Window {
    HWND hwnd;
    bool shouldClose = false;
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

// Since your typedefs are uintptr_t, we'll store pointers cast to uintptr_t and back
platform_context_t platform_init() {
    Win32Context* ctx = new Win32Context;
    ctx->hInstance = GetModuleHandle(NULL);

    // Register a window class once here
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DefWindowProc;  // You might want your own WndProc or just DefWindowProc for now
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
    // On Win32, primary screen can be represented by the primary monitor handle or just a dummy value
    // For simplicity, just return 1, since your API only needs a placeholder
    return 1;
}

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height) {
    Win32Context* ctx = reinterpret_cast<Win32Context*>(context);
    if (!ctx) return 0;

    // Create the window with your registered class
    HWND hwnd = CreateWindowEx(
        0,
        MAKEINTATOM(ctx->classAtom),
        "",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        (int)width, (int)height,
        NULL,
        NULL,
        ctx->hInstance,
        NULL
    );

    if (!hwnd) return 0;

    Win32Window* win = new Win32Window;
    win->hwnd = hwnd;

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
    if (!win) return false;
    return win->shouldClose;
}

bool platform_set_title(platform_context_t context, platform_window_t window, const char* title) {
    Win32Window* win = reinterpret_cast<Win32Window*>(window);
    if (!win || !win->hwnd) return false;

    return SetWindowTextA(win->hwnd, title) != 0;
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
