#ifndef FONTS_H
#define FONTS_H

#include "graphics.h"
#include "stb_truetype.h"

#define MAX_FONTS_CACHE 16

typedef struct sFont {
    char fontName[256];
    float fontSize;
    stbtt_bakedchar cdata[96];
    unsigned int texture;
} Font;

typedef struct sFontCache {
    Font* fonts[MAX_FONTS_CACHE];
    size_t count;
} FontCache;

Font* loadFont(char* fontName, float fontSize);
void destroyFontCache(void);

#endif // FONTS_H