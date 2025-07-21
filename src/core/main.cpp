#include <xcb/xcb.h>
#include <string.h>
#include <iostream>

int main() {
    // Connect to the X server
    xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) {
        std::cerr << "Cannot open display\n";
        return 1;
    }

    // Get the first screen
    const xcb_setup_t *setup = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen = iter.data;

    // Create a window
    xcb_window_t window = xcb_generate_id(connection);
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[2] = {screen->black_pixel, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS};

    xcb_create_window(connection,
                      XCB_COPY_FROM_PARENT,      // depth
                      window,                   // window Id
                      screen->root,             // parent window
                      0, 0,                    // x, y
                      400, 300,                // width, height
                      10,                      // border width
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      mask, values);

    
    const char *title = "Spikehead thingy";
    xcb_change_property(
        connection,
        XCB_PROP_MODE_REPLACE,
        window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(title),
        title);
    
    // For modern window managers, also set _NET_WM_NAME with UTF8_STRING atom
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_NAME"), "_NET_WM_NAME");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);
    if (reply) {
        xcb_change_property(
            connection,
            XCB_PROP_MODE_REPLACE,
            window,
            reply->atom,
            XCB_ATOM_STRING,
            8,
            strlen(title),
            title);
        free(reply);
    }

    // Map (show) the window
    xcb_map_window(connection, window);
    xcb_flush(connection);

    // Event loop
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_EXPOSE:
                // Window exposed (redraw)
                break;
            case XCB_KEY_PRESS:
                // Exit on any key press
                free(event);
                xcb_disconnect(connection);
                return 0;
        }
        free(event);
    }

    xcb_disconnect(connection);
    return 0;
}
