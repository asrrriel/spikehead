#include "platform.h"
#include "glad/include/glad/glad.h"
#include "GL/gl.h"
#include "sys/renderer.h"
#include "ecs.h"
#include "gl_abstraction/vbo.h"
#include "gl_abstraction/vao.h"
#include "gl_abstraction/ibo.h"
#include "gl_abstraction/shader.h"
#include "nothings/stb_image.h"
#include "sys/assets.h"
#include <iostream>
#include <sys/types.h>
#include <vector>

extern Shader* color_shader;
extern Shader* texture_shader;
extern VAO   * square_vao;
extern IBO   * square_ibo;

void init_defaults();

void __renderer_print_errors(){
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error: " << err << "\n";
    }
}

platform_gl_context_t main_context = 0;

platform_gl_context_t renderer_init(platform_context_t context, platform_window_t window){
    platform_gl_context_t gl_context = 0;


    if(!main_context){
        gl_context = platform_create_gl_context(context, window);
        main_context = gl_context;
    } else{
        gl_context = platform_create_gl_context(context, window, main_context);
    }

    platform_make_context_current(gl_context);

    if(!gladLoadGL()){
        std::cerr << "Failed to initialize GLAD\n";
        return 0;
    }
   
    const char* vendor = (const char*)glGetString(GL_VERSION);
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout << "OpenGL version: " <<  major << '.' << minor << '\n';
    std::cout << "OpenGL vendor: " << vendor << '\n'; 

    init_defaults();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    __renderer_print_errors();

    return gl_context;
}
bool renderer_setbgcol(float r, float g, float b, float a){
    glClearColor(r, g, b, a);
    __renderer_print_errors();
    return true;
}
bool renderer_clear(){
    glClear(GL_COLOR_BUFFER_BIT);
    __renderer_print_errors();
    return true;
}


void* renderer_create_color_material(float r, float g, float b){
    color_material_t* mat = new color_material_t();

    float color[] = {r, g, b};

    mat->color = Vec3f(color);

    return reinterpret_cast<void*>(mat);
}

void* renderer_create_texture_material(std::string texture){
    texture_material_t* mat = new texture_material_t();

    platform_gl_context_t gl_context = platform_get_current_gl_context();
    platform_make_context_current(main_context);

    asset_descriptor_t asset = lookup_asset(texture);

    if(asset.error){
        std::cerr << "Failed to find texture '" << texture << "'\n";
        return nullptr;
    }

    if(asset.type != ASSET_TYPE_IMAGE){
        std::cerr << "Asset '" << texture << "' is not a texture\n";
        return nullptr;
    }

    stbi_set_flip_vertically_on_load_thread(true);
    int width, height;
    unsigned char* data = stbi_load(asset.path.c_str(), &width, &height, NULL, 4);

    if(!data){
        std::cerr << "Failed to load texture '" << texture << "'\n";
        return nullptr;
    }

    mat->texture = new Texture(data, width, height, GL_NEAREST);

    stbi_image_free(data);

    platform_make_context_current(gl_context);

    return reinterpret_cast<void*>(mat);
}

void __render_material(Entity e, VAO* vao, GLuint index_count){
    if(e.has_component(COMP_TYPE_MAT_COLOR)){
        color_material_t* c = (color_material_t*)e.get_component(COMP_TYPE_MAT_COLOR);
        if(!c){
            std::cout << "Invalid color material\n";
            return;
        }
        color_shader->Bind();
        color_shader->SetUniform3f("color", c->color);
    } else if(e.has_component(COMP_TYPE_MAT_TEXTURE)){
        texture_material_t* t = (texture_material_t*)e.get_component(COMP_TYPE_MAT_TEXTURE);
        if(!t){
            std::cout << "Invalid texture material\n";
            return;
        }
        t->texture->Bind(0);
        texture_shader->Bind();
        texture_shader->SetUniform1i("texture1", 0);
    } else {
        std::cout << "No material\n";
        return;
    }

    vao->Bind();
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    __renderer_print_errors();
    vao->Unbind();
}

void renderer_draw(std::vector<Entity> entities){
    for (auto e : entities) {
        if(e.has_component(COMP_TYPE_SQUARE)){
            __render_material(e, square_vao, 6);
        }
    } 
}