#ifndef TEXT_H
#define TEXT_H

#include "graphics.h"
#include "shaders.h"
#include "fonts.h"
//#include "stb_truetype.h"

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
    bool visible;
    float proj[16];
    float model[16];
    Color color;
    size_t vertsCount;
    float x , y;
    Font* font;
    char txt[MAX_TEXT];

    GLuint vao;
    GLuint vbo;
    GLint locProj;
    GLint locTexture;
    GLint locColor;
    GlyphVertex* verts;
} Text;

Text* createStaticText(char* str, char* fontName, float fontSize, float x, float y);
Text* createText(char* str, char* fontName, float fontSize, float x, float y);
void moveText(Text* t, float x, float y);
void setText(Text* t, char* str);
void changeText(Text* t, int pos, char* str);
void setTextColor(Text* t, Color cl);//float r, float g, float b);
void destroyText(Text* t);
void drawText(Text* t, ColoredTextureShader* shader);

#endif // TEXT_H