#ifndef SHADERS_H
#define SHADERS_H

#include "graphics.h"

typedef struct sGameShader {
    GLuint program;
    GLint locProj;
    GLint locModel;
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

#endif