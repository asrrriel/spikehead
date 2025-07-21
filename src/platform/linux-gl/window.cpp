#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <iostream>
#include "window.h"
#include <string.h>

xcb_atom_t wm_delete_window_atom = 0;


platform_context_t platform_init() {
    xcb_connection_t* conn = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(conn)) {
        std::cerr << "Cannot open display\n";
        return 0;
    }

    xcb_intern_atom_cookie_t wm_delete_window_cookie = xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");

    xcb_intern_atom_reply_t *wm_delete_window_reply = xcb_intern_atom_reply(conn, wm_delete_window_cookie, nullptr);

    xcb_atom_t wm_delete_window_atom = wm_delete_window_reply->atom;

    free(wm_delete_window_reply);


    return reinterpret_cast<platform_context_t>(conn);
}

void platform_deinit(platform_context_t context) {
    xcb_disconnect(reinterpret_cast<xcb_connection_t*>(context));
}

platform_screen_t platform_get_primary_screen(platform_context_t context) {
    xcb_connection_t* connection = reinterpret_cast<xcb_connection_t*>(context);
    const xcb_setup_t* setup = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    return reinterpret_cast<platform_screen_t>(iter.data);
}

platform_window_t platform_create_window(platform_context_t context, platform_screen_t screen, std::size_t width, std::size_t height) {
    xcb_connection_t* connection = reinterpret_cast<xcb_connection_t*>(context);
    xcb_screen_t* scr = reinterpret_cast<xcb_screen_t*>(screen);

    xcb_window_t window = xcb_generate_id(connection);

    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[2] = {
        scr->black_pixel,
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY
    };

    xcb_create_window(connection,
                      XCB_COPY_FROM_PARENT,
                      window,
                      scr->root,
                      0, 0,
                      static_cast<uint16_t>(width),
                      static_cast<uint16_t>(height),
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      scr->root_visual,
                      mask,
                      values);


    return (platform_window_t)(window);
}

bool platform_set_title(platform_context_t context, platform_window_t window, const char* title) {
    xcb_connection_t* connection = reinterpret_cast<xcb_connection_t*>(context);
    xcb_window_t win = static_cast<xcb_window_t>(window);

    xcb_change_property(connection,
                        XCB_PROP_MODE_REPLACE,
                        win,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        strlen(title),
                        title);
    return true;
}

bool platform_should_close(platform_context_t context, platform_window_t window) {
    xcb_generic_event_t *event;
    while ((event = xcb_poll_for_event((xcb_connection_t*)context)) != nullptr) {
        if ((event->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
            xcb_client_message_event_t *cm = (xcb_client_message_event_t *)event;
            if (cm->data.data32[0] == wm_delete_window_atom) {
                free(event);
                return true;
            }
        }
        free(event);
    }
    return false;
}


void platform_show_window(platform_context_t context, platform_window_t window) {
    xcb_connection_t* connection = reinterpret_cast<xcb_connection_t*>(context);
    xcb_map_window(connection, static_cast<xcb_window_t>(window));
    xcb_flush(connection);
}

void platform_destroy_window(platform_context_t context, platform_window_t window) {
    xcb_connection_t* connection = reinterpret_cast<xcb_connection_t*>(context);
    xcb_destroy_window(connection, static_cast<xcb_window_t>(window));
}
