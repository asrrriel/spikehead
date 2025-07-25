#include <string>
#include "gl_abstraction/shader.h"
#include "gl_abstraction/vao.h"
#include "gl_abstraction/vbo.h"
#include "gl_abstraction/ibo.h"

static std::string color_vert = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
void main() {
    gl_Position = vec4(aPos, 1.0, 1.0);
}
)";

static std::string color_frag = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

static std::string texture_vert = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0, 1.0);
}
)";

static std::string texture_frag = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoords) * vec4(1.0, 1.0, 1.0, 0.6);
}
)";

Shader* color_shader = nullptr;
Shader* texture_shader = nullptr;
VAO   * square_vao = nullptr;
IBO   * square_ibo = nullptr;

void init_defaults(){
    //MAT_COLOR
    color_shader = new Shader(color_vert, color_frag);

    //MAT_TEXTURE
    texture_shader = new Shader(texture_vert, texture_frag);

    //SQUARE
    float vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    
    square_vao = new VAO();
    square_vao->Bind();
    VBO square_vbo(vertices, sizeof(vertices));
    square_ibo = new IBO(indices, sizeof(indices));
    square_vbo.Bind();
    square_ibo->Bind();
    square_vao->LinkAttrib(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    square_vao->LinkAttrib(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    square_vao->Unbind();
    square_vbo.Unbind();
    square_ibo->Unbind();
}