#include "platform.h"
#include "sys/renderer.h"
#include "sys/assets.h"
#include "math.h"
#include <iostream>
#include <chrono>
#include <thread>

const char* vertexShaderSrc = R"glsl(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aUV;

out vec2 UV;

void main(){
    UV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

const char* fragmentShaderSrc = R"glsl(
    #version 330 core
    in vec2 UV;
    out vec4 FragColor;
    uniform sampler2D texture1;
    void main() { FragColor = texture(texture1, UV); }
)glsl";

int main() {
    platform_context_t ctx = platform_init();
    if (!ctx) {
        return 1;
    }

    platform_screen_t screen = platform_get_primary_screen(ctx);
    platform_window_t window = platform_create_window(ctx, screen, 800, 600);
    platform_set_title(ctx, window, "Hello " PLATFORM "!");
    platform_show_window(ctx, window);

    if(!renderer_init(ctx, window)){
        exit(69);
    }

    if(!load_asset_pack(find_asset_pack())){
        exit(42);
    }

    asset_descriptor_t test_img = lookup_asset("test_img");

    std::cout << "test_img error: " << test_img.error << std::endl;
    std::cout << "test_img type: " << test_img.type << std::endl;
    std::cout << "test_img path: " << test_img.path << std::endl;

    // 4 vertices, each with pos (x,y,z) and normal (nx,ny,nz)
    float vertices[] = {
        // positions        // normals
        0.5f,  0.5f,  1.0f, 1.0f,  // top right
        -0.5f,  0.5f,  0.0f, 1.0f,  // top left
        -0.5f, -0.5f,  0.0f, 0.0f,  // bottom left
        0.5f, -0.5f,  1.0f, 0.0f,  // bottom right
    };

    // 6 indices for 2 triangles forming the square
    unsigned int indices[] = {
        0, 1, 2,
        0, 3, 2
    };

    uint8_t texture[4*4] = {
        255,0,0,255, // top left pixel
        0,0,0,255, // top right pixel
        0,0,0,255, // bottom left pixel
        255,0,0,255, // bottom right pixel
    };

    renderer_unbatched_object_t obj = create_object_textured(
        vertices,
        16,
        indices,
        6,
        create_shader(vertexShaderSrc, fragmentShaderSrc),
        load_texture(test_img),
        "texture1"
    );
    
    while(!platform_should_close(ctx, window)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        auto now = std::chrono::steady_clock::now();
        auto duration = now.time_since_epoch();

        double seconds = duration_cast<std::chrono::duration<double>>(duration).count();

        float hue = std::fmod(seconds, 3.1415);
        
        float r = fabs(sin(hue));
        float g = fabs(sin(hue + 2.094));
        float b = fabs(sin(hue + 4.188));
        
        renderer_setbgcol(r,g,b);
        renderer_clear();

        renderer_draw_object(obj);

        renderer_swap();
    }

    renderer_deinit();
    platform_destroy_window(ctx, window);
    platform_deinit(ctx);

    return 0;
}
