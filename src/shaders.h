#ifndef SHADERS_H
#define SHADERS_H

#include <GLES3/gl3.h>

typedef struct sSimpleShader {
    GLuint program;
    GLint locProj;
    GLint locModel;
} SimpleShader;

typedef struct sGlowShader {
    GLuint program;
    GLint locProj;
    GLint locModel;
    GLfloat locTime;
} GlowShader;

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

SimpleShader* createSimpleShader(void);
GlowShader* createGlowShader(void);
TextureShader* createTextureShader(void);
ColoredTextureShader* createColoredTextureShader(void);

void useProgram(GLuint shader);

void destroySimpleShader(SimpleShader* shader);
void destroyGlowShader(GlowShader* shader);
void destroyTextureShader(TextureShader* shader);
void destroyColoredTextureShader(ColoredTextureShader* shader);

#endif