#ifndef SHADERS_H
#define SHADERS_H

#include "graphics.h"

extern const char* default_VS;
extern const char* default_FS;
extern const char* glow_FS;
extern const char* simple_textured_quad_VS;
extern const char* simple_textured_quad_FS;

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