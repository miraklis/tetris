#ifndef TEXT_H
#define TEXT_H

#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "fonts.h"

#define MAX_TEXT 256

enum TextType {
    TXTTYPE_STATIC = 0,
    TXTTYPE_DYNAMIC,
    TXTTYPE_COUNT
};

typedef struct Text {
    Object base;
    enum TextType type;
    bool visible;
    float model[16];
    Color color;
    uint32_t vertCount;
    Font* font;
    char txt[MAX_TEXT];

    GLuint vao;
    GLuint vbo;
    GLint locProj;
    GLint locTexture;
    GLint locColor;
    VertexGlyph* verts;
} Text;
static_assert(offsetof(Text, base) == 0, "Text struct not aligned for base object" );

Text* createStaticText(char* str, char* fontName, float fontSize, float x, float y);
Text* createText(char* str, char* fontName, float fontSize, float x, float y);
void moveText(Text* t, float x, float y);
void setText(Text* t, char* str);
void changeText(Text* t, int pos, char* str);
void setTextColor(Text* t, Color cl);//float r, float g, float b);
void destroyText(Text* t);
void drawText(RenderContext* ctx, Text* text);

#endif // TEXT_H