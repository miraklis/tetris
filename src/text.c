#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "text.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static unsigned char* loadTTFFont(const char* fontName)
{
    FILE* f = fopen(fontName, "rb");
    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* ttBuffer = malloc(fileSize);
    fread(ttBuffer, 1, fileSize, f);
    fclose(f);
    return ttBuffer;
}

static GLuint loadTTFTexture(unsigned char bitmap[512*512])
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        512,
        512,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        bitmap
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

    return tex;
}

Text* createStaticText(char* str, const char* fontName, float fontSize, float x, float y)
{
    Text* t = (Text*)malloc(sizeof(Text));
    t->type = TXTTYPE_STATIC;
    t->font = loadTTFFont(fontName);
    t->fontSize = fontSize;
    t->x = x;
    t->y = y;
    unsigned char bitmap[512*512];
    stbtt_BakeFontBitmap(t->font, 0, t->fontSize, bitmap, 512, 512, 32, 96, t->cdata);
    t->texture = loadTTFTexture(bitmap);
    size_t charCount = slength(str, MAX_TEXT);//strnlen(str, MAX_TEXT);
    t->verts = (GlyphVertex*)calloc(sizeof(GlyphVertex), 6 * charCount);

    int count=0;
    int charIt = 0;
    float yy = y + fontSize;
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->cdata, 512, 512, c - 32,
            &x, &yy, &q, 1);
        t->verts[count++] = (GlyphVertex){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertsCount = count;

    glGenVertexArrays(1,&t->vao);
    glGenBuffers(1,&t->vbo);
    glBindVertexArray(t->vao);
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GlyphVertex)*t->vertsCount, t->verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(GlyphVertex),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GlyphVertex),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    orthoMatrix(0, 1920, 1080, 0, -1, 1, t->proj);
    t->color = (Color){1.0f, 1.0f, 1.0f, 1.0f};
    t->visible = true;
    return t;
}

Text* createText(char* str, const char* fontName, float fontSize, float x, float y)
{
    Text* t = (Text*)malloc(sizeof(Text));
    t->type = TXTTYPE_DYNAMIC;
    t->font = loadTTFFont(fontName);
    t->fontSize = fontSize;
    t->x = x;
    t->y = y;
    unsigned char bitmap[512*512];
    stbtt_BakeFontBitmap(t->font, 0, t->fontSize, bitmap, 512, 512, 32, 96, t->cdata);
    t->texture = loadTTFTexture(bitmap);
    memset(t->txt, 0, MAX_TEXT);
    t->verts = (GlyphVertex*)calloc(sizeof(GlyphVertex), 6 * MAX_TEXT);

    int count=0;
    int charIt = 0;
    float yy = y + fontSize;
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->cdata, 512, 512, c - 32,
            &x, &yy, &q, 1);
        t->verts[count++] = (GlyphVertex){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertsCount = count;
    glGenVertexArrays(1,&t->vao);
    glGenBuffers(1,&t->vbo);
    glBindVertexArray(t->vao);
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GlyphVertex) * 6 * MAX_TEXT, t->verts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(GlyphVertex),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GlyphVertex),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    orthoMatrix(0, dm->w, dm->h, 0, -1, 1, t->proj);
    t->color = (Color){1.0f, 1.0f, 1.0f, 1.0f};
    t->visible = true;
    return t;
}

void setText(Text* t, char* str)
{
    if(t == NULL || t->type == TXTTYPE_STATIC || t->verts == NULL)
        return;

    float x = t->x;
    int count=0;
    int charIt = 0;
    float yy = t->y + t->fontSize;
    memset(t->txt, 0, MAX_TEXT);
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->cdata, 512, 512, c - 32,
            &x, &yy, &q, 1);
        t->verts[count++] = (GlyphVertex){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertsCount = count;
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GlyphVertex)*t->vertsCount, t->verts);
}

void changeText(Text* t, int pos, char* str)
{
    if(t == NULL || t->type == TXTTYPE_STATIC || t->verts == NULL)
        return;

    float x = t->x + (pos * t->fontSize);
    float y = t->y;
    float yy = y + t->fontSize;
    int count=pos * 6;
    int charIt = 0;
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->cdata, 512, 512, c - 32,
            &x, &yy, &q, 1);
        t->verts[count++] = (GlyphVertex){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertsCount = count;
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GlyphVertex) * 6 * pos, sizeof(GlyphVertex) * 6 * charIt, &t->verts[pos * 6]);
}

void setTextColor(Text* t, float r, float g, float b)
{
    if(t == NULL)
        return;
    t->color.r = r;
    t->color.g = g;
    t->color.b = b;
}

void setFontSize(Text* t, float fontSize)
{
    if(t == NULL)
        return;
    t->fontSize = fontSize;
    unsigned char bitmap[512*512];
    stbtt_BakeFontBitmap(t->font, 0, t->fontSize, bitmap, 512, 512, 32, 96, t->cdata);
    t->texture = loadTTFTexture(bitmap);

    float x = t->x;
    float y = t->y;
    float yy = y + fontSize;
    char s[MAX_TEXT];
    strncpy(s, t->txt, MAX_TEXT);
    int count = 0;
    int charIt = 0;
    while(s[charIt])
    {
        unsigned char c = s[charIt];
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->cdata, 512, 512, c - 32,
            &x, &yy, &q, 1);
        t->verts[count++] = (GlyphVertex){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertsCount = count;
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GlyphVertex)*t->vertsCount, t->verts);    
}

void moveText(Text* t, float x, float y)
{
    int count=0;
    int charIt = 0;
    t->x = x;
    t->y = y;
    float yy = y + t->fontSize;
    char* str = &t->txt[0];
    while(*str)
    {
        unsigned char c = *str++;
        t->txt[charIt++] = c;
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            t->cdata, 512, 512, c - 32,
            &x, &yy, &q, 1);
        t->verts[count++] = (GlyphVertex){q.x0,q.y0,q.s0,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x0,q.y1,q.s0,q.t1};
        t->verts[count++] = (GlyphVertex){q.x1,q.y0,q.s1,q.t0};
        t->verts[count++] = (GlyphVertex){q.x1,q.y1,q.s1,q.t1};
    }
    t->vertsCount = count;
    glBindBuffer(GL_ARRAY_BUFFER,t->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GlyphVertex)*t->vertsCount, t->verts);
}

void destroyText(Text* t)
{
    if(t == NULL)
        return;
    glDeleteTextures(1, &t->texture);
    glDeleteBuffers(1, &t->vbo);
    glDeleteVertexArrays(1, &t->vao);
    free(t->font);
    free(t->verts);
    free(t);
    t = NULL;
}

void drawText(Text* t, ColoredTextureShader* shader)
{
    if(t == NULL || !t->visible)
        return;

    useProgram(shader->program);
    glBindVertexArray(t->vao);
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, t->proj);
    glUniform3f(shader->locColor, t->color.r, t->color.g, t->color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t->texture);
    glUniform1i(shader->locTexture, 0);
    glDrawArrays(GL_TRIANGLES, 0, t->vertsCount);
}
