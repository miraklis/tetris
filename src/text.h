#ifndef TEXT_H
#define TEXT_H

#include <stddef.h>
#include <GLES3/gl3.h>
#include <SDL3/SDL.h>
#include "shaders.h"
#include "stb_truetype.h"

#define MAX_TEXT 256

enum TextType {
    TXTTYPE_STATIC = 0,
    TXTTYPE_DYNAMIC,
    TXTTYPE_COUNT
};

typedef struct GlyphVertex {
    float x,y;
    float u,v;
} GlyphVertex;

typedef struct Text {
    enum TextType type;
    GLuint vao;
    GLuint vbo;
    GLint locProj;
    GLint locTexture;
    GLint locColor;
    float proj[16];
    GLuint texture;
    SDL_FColor color;
    size_t vertsCount;
    float x , y;
    float fontSize;
    stbtt_bakedchar cdata[96];
    unsigned char* font;
    char txt[MAX_TEXT];
    GlyphVertex* verts;
} Text;

void initStaticText(Text* t, char* str, const char* fontName, float fontSize, float x, float y);
void initText(Text* t, char* str, const char* fontName, float fontSize, float x, float y);
void moveText(Text* t, float x, float y);
void setText(Text* t, char* str);
void changeText(Text* t, int pos, char* str);
void setTextColor(Text* t, float r, float g, float b);
void setFontSize(Text* t, float fontSize);
void drawText(Text* t, ColoredTextureShader* shader);//GLuint program);

#endif // TEXT_H