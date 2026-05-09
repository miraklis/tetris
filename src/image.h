#ifndef IMAGE_H
#define IMAGE_H

#include "graphics.h"
#include "shaders.h"

typedef struct sImage {
    float x,y;
    int width;
    int height;
    int channels;
    float proj[16];
    float model[16];
    bool visible;
    unsigned char* data;

    GLuint textureID;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} Image;

Image* loadImage(char* path);
void moveImage(Image* img, float x, float y);
void drawImage(Image* img, TextureShader* shader);
void destroyImage(Image* img);

#endif // IMAGE_H