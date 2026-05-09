#include "std.h"
#include <stdio.h>
#include <string.h>
#include "fonts.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static FontCache fontCache = {0};

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

static int checkExists(char fontName[256], float fontSize)
{
    for(int i=0; i < fontCache.count; i++) {
        if(fontCache.fonts[i] != NULL 
            && strcmp(fontCache.fonts[i]->fontName, fontName) == 0 
            && fontCache.fonts[i]->fontSize == fontSize) {
                return i;
        }
    }
    return -1;
}

Font* loadFont(char fontName[256], float fontSize)
{
    // Check if the font exists in cache
    int idx = checkExists(fontName, fontSize);
    if(idx != -1) {
        return fontCache.fonts[idx];
    }
    // Check if we can load more fonts
    if(fontCache.count >= MAX_FONTS_CACHE) {
        fprintf(stderr, "Font cache is full. Cannot load more fonts.\n");
        return NULL;
    }
    // Load new font and add to cache
    Font* font = (Font*)malloc(sizeof(Font));
    memset(font->fontName, 0, 256);
    sprintf(font->fontName, "%s", fontName);
    font->fontSize = fontSize;
    unsigned char* fontData = loadTTFFont(fontName);
    unsigned char bitmap[512*512];
    stbtt_BakeFontBitmap(fontData, 0, font->fontSize, bitmap, 512, 512, 32, 96, font->cdata);
    font->texture = loadTTFTexture(bitmap);
    fontCache.fonts[fontCache.count++] = font;
    return font;
}

static void destroyFont(Font* font)
{
    if(font == NULL)
        return;
    glDeleteTextures(1, &font->texture);
    FREE(font);
}

void destroyFontCache(void)
{
    for(int i=0; i < fontCache.count; i++) {
        if(fontCache.fonts[i] != NULL) {
            destroyFont(fontCache.fonts[i]);
        }
    }
    fontCache.count = 0;
}