#ifndef SHADERS_H
#define SHADERS_H

#include "graphics.h"

#define MAX_UNIFORMS 10

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
    GLint locTexture;
} TextureShader;

typedef struct sColoredTextureShader {
    GLuint program;
    GLint locProj;
    GLint locColor;
    GLint locTexture;
} ColoredTextureShader;

GameShader* createGameShader(void);
TextureShader* createTextureShader(void);
ColoredTextureShader* createColoredTextureShader(void);

void useProgram(GLuint shader);


// typedef enum eProgramId {
//     PRG_DEFAULT = 0,
//     PRG_DEFAULT_TEXT,
//     PRG_COUNT
// } ProgramId;

// typedef struct sProgram {
//     GLuint shaderProgram;
//     ProgramId id;
//     GLint uniformLocation[MAX_UNIFORMS];
// } Program;

// extern GLuint programsPool[PRG_COUNT];
// extern GLint uniformsLocations[PRG_COUNT][MAX_UNIFORMS];

//void createProgram(const char* vs, const char* fs, ProgramId id);
//Program* createProgram(const char* vs, const char* fs, ProgramId id, char* uniforms[MAX_UNIFORMS]);
//void useProgram(GLuint shader);

#endif