#include "std.h"
#include "shaders.h"

static GLuint currentProgram;

const char* default_VS = 
"#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"layout (location = 1) in vec4 color;\n"
"layout (location = 2) in float glow;\n"
"uniform mat4 proj;\n"
"uniform mat4 model;\n"
"out vec4 vColor;\n"
"out float vGlow;\n"
"void main()\n"
"{\n"
"    gl_Position = proj * model * vec4(position, 0.0, 1.0);\n"
"    vColor = color;\n"
"    vGlow = glow;\n"
"}";

// const char* default_FS = 
// "#version 330 core\n"
// "in vec4 vColor;\n"
// "out vec4 FragColor;\n"
// "void main()\n"
// "{\n"
// "    FragColor = vColor;\n"
// "}";

const char* default_FS = 
"#version 330 core\n"
"in vec4 vColor;\n"
"in float vGlow;\n"
"out vec4 FragColor;\n"
"uniform float time;\n"
"void main()\n"
"{\n"
"    float pulse = sin(time * 8.0) * 0.5 + 0.5;\n"
"    vec3 glowColor = vColor.rgb + vec3(vGlow * pulse * 0.5);\n"
"    FragColor = vec4(glowColor, vColor.a);\n"
"}";

const char* texture_VS = 
"#version 330 core\n"
"layout(location=0) in vec2 position;\n"
"layout(location=1) in vec2 uv;\n"
"uniform mat4 proj;\n"
"uniform mat4 model;\n"
"out vec2 vUV;\n"
"void main()\n"
"{\n"
"    gl_Position = proj * model * vec4(position, 0.0, 1.0);\n"
"    vUV = uv;\n"
"}";

const char* texture_FS = 
"#version 330 core\n"
"in vec2 vUV;\n"
"out vec4 FragColor;\n"
"uniform sampler2D samplerTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(samplerTexture, vUV);\n"
"}";

const char* colored_texture_FS = 
"#version 330 core\n"
"in vec2 vUV;\n"
"out vec4 FragColor;\n"
"uniform sampler2D samplerTexture;\n"
"uniform vec3 color;\n"
"void main()\n"
"{\n"
"    vec4 sampled = texture(samplerTexture, vUV);\n"
"    FragColor = vec4(color, sampled.a);\n"
"}";

static GLuint compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

GameShader* createGameShader(void)
{
    GameShader* shader = (GameShader*)malloc(sizeof(GameShader));

    shader->program = glCreateProgram();
    GLuint v = compileShader(GL_VERTEX_SHADER, default_VS);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, default_FS);
    
    glAttachShader(shader->program, v);
    glAttachShader(shader->program, f);
    glLinkProgram(shader->program);

    glDeleteShader(v);
    glDeleteShader(f);

    shader->locProj = glGetUniformLocation(shader->program, "proj");
    shader->locModel = glGetUniformLocation(shader->program, "model");
    shader->locTime = glGetUniformLocation(shader->program, "time");

    return shader;
}

TextureShader* createTextureShader(void)
{
    TextureShader* shader = (TextureShader*)malloc(sizeof(TextureShader));

    shader->program = glCreateProgram();
    GLuint v = compileShader(GL_VERTEX_SHADER, texture_VS);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, texture_FS);
    
    glAttachShader(shader->program, v);
    glAttachShader(shader->program, f);
    glLinkProgram(shader->program);

    glDeleteShader(v);
    glDeleteShader(f);

    shader->locProj = glGetUniformLocation(shader->program, "proj");
    shader->locModel = glGetUniformLocation(shader->program, "model");
    shader->locTexture = glGetUniformLocation(shader->program, "samplerTexture");

    return shader;
}

ColoredTextureShader* createColoredTextureShader(void)
{
    ColoredTextureShader* shader = (ColoredTextureShader*)malloc(sizeof(ColoredTextureShader));

    shader->program = glCreateProgram();
    GLuint v = compileShader(GL_VERTEX_SHADER, texture_VS);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, colored_texture_FS);
    
    glAttachShader(shader->program, v);
    glAttachShader(shader->program, f);
    glLinkProgram(shader->program);

    glDeleteShader(v);
    glDeleteShader(f);

    shader->locProj = glGetUniformLocation(shader->program, "proj");
    shader->locModel = glGetUniformLocation(shader->program, "model");
    shader->locColor = glGetUniformLocation(shader->program, "color");
    shader->locTexture = glGetUniformLocation(shader->program, "samplerTexture");

    return shader;
}

void useProgram(GLuint program) {
    if(program != currentProgram) {
        currentProgram = program;
        glUseProgram(program);
    }
}

