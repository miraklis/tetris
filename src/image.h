#ifndef IMAGE_H
#define IMAGE_H

#include <GLES3/gl3.h>
#include "shaders.h"

typedef struct sImage {
    int width;
    int height;
    int channels;
    float proj[16];
    bool visible;
    unsigned char* data;

    GLuint textureID;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} Image;

Image* loadImage(char* path);
void drawImage(Image* img, TextureShader* shader);

#endif // IMAGE_H