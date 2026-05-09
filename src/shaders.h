#ifndef SHADERS_H
#define SHADERS_H

#include "graphics.h"
#include <GLES3/gl3.h>

typedef struct sGameShader {
    GLuint program;
    GLint locProj;
    GLint locModel;
    GLfloat locTime;
} GameShader;

typedef struct sTextureShader {
    GLuint program;
    GLint locProj;
    GLint locModel;
    GLint locTexture;
} TextureShader;

typedef struct sColoredTextureShader {
    GLuint program;
    GLint locProj;
    GLint locModel;
    GLint locColor;
    GLint locTexture;
} ColoredTextureShader;

GameShader* createGameShader(void);
TextureShader* createTextureShader(void);
ColoredTextureShader* createColoredTextureShader(void);

void useProgram(GLuint shader);

void destroyGameShader(GameShader* shader);
void destroyTextureShader(TextureShader* shader);
void destroyColoredTextureShader(ColoredTextureShader* shader);

#endif