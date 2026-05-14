#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "text.h"
#include "fonts.h"

Text* createStaticText(char* str, char* fontName, float fontSize, float x, float y)
{
    Text* t = (Text*)malloc(sizeof(Text));
    
    t->base.id = OBJ_TYPE_Text;
    t->base.x = x;
    t->base.y = y;
    t->base.width = 0.0f;
    t->base.height = 0.0f;
    t->type = TXTTYPE_STATIC;
    t->font = loadFont(fontName, fontSize);
    size_t charCount = slength(str, MAX_TEXT);//strnlen(str, MAX_TEXT);
    t->verts = (VertexGlyph*)calloc(sizeof(VertexGlyph), 6 * charCount);

    int count=0;
    int charIt = 0;
    float xx = 0.0f;
    float yy = fontSize;
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->font->cdata, 512, 512, c - 32,
            &xx, &yy, &q, 1);
        t->verts[count++] = (VertexGlyph){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertCount = count;

    glGenVertexArrays(1,&t->vao);
    glGenBuffers(1,&t->vbo);
    glBindVertexArray(t->vao);
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexGlyph)*t->vertCount, t->verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(VertexGlyph),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(VertexGlyph),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    translateMatrix(x, y, t->model);
    t->color = palette.colorWhite;
    t->visible = true;
    return t;
}

Text* createText(char* str, char* fontName, float fontSize, float x, float y)
{
    Text* t = (Text*)malloc(sizeof(Text));
    t->type = TXTTYPE_DYNAMIC;
    t->base.x = x;
    t->base.y = y;
    t->font = loadFont(fontName, fontSize);
    memset(t->txt, 0, MAX_TEXT);
    t->verts = (VertexGlyph*)calloc(sizeof(VertexGlyph), 6 * MAX_TEXT);

    int count=0;
    int charIt = 0;
    float xx = 0.0f;
    float yy = fontSize;
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->font->cdata, 512, 512, c - 32,
            &xx, &yy, &q, 1);
        t->verts[count++] = (VertexGlyph){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertCount = count;
    glGenVertexArrays(1,&t->vao);
    glGenBuffers(1,&t->vbo);
    glBindVertexArray(t->vao);
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexGlyph) * 6 * MAX_TEXT, t->verts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(VertexGlyph),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(VertexGlyph),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    translateMatrix(x, y, t->model);
    t->color = palette.colorWhite;
    t->visible = true;
    return t;
}

void setText(Text* t, char* str)
{
    if(t == NULL || t->type == TXTTYPE_STATIC || t->verts == NULL)
        return;

    int count=0;
    int charIt = 0;
    float xx = 0.0f;
    float yy = t->font->fontSize;
    memset(t->txt, 0, MAX_TEXT);
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->font->cdata, 512, 512, c - 32,
            &xx, &yy, &q, 1);
        t->verts[count++] = (VertexGlyph){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertCount = count;
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexGlyph)*t->vertCount, t->verts);
}

void changeText(Text* t, int pos, char* str)
{
    if(t == NULL || t->type == TXTTYPE_STATIC || t->verts == NULL)
        return;

    float xx = (pos * t->font->fontSize);
    float yy = t->font->fontSize;
    int count=pos * 6;
    int charIt = 0;
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->font->cdata, 512, 512, c - 32,
            &xx, &yy, &q, 1);
        t->verts[count++] = (VertexGlyph){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (VertexGlyph){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (VertexGlyph){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertCount = count;
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(VertexGlyph) * 6 * pos, sizeof(VertexGlyph) * 6 * charIt, &t->verts[pos * 6]);
}

void setTextColor(Text* t, Color cl)
{
    if(t == NULL)
        return;
    t->color = cl;
}

void moveText(Text* t, float x, float y)
{

    t->base.x = x;
    t->base.y = y;
    translateMatrix(x, y, t->model);
}

void destroyText(Text* t)
{
    if(t == NULL)
        return;
    glDeleteBuffers(1, &t->vbo);
    glDeleteVertexArrays(1, &t->vao);
    FREE(t->verts);
    FREE(t);
}

void drawText(RenderContext* ctx, Text* text)
{
    if(text == NULL || !text->visible)
        return;
    renderContextQueueOject(ctx, RENDERABLE_TEXT, text);
}
