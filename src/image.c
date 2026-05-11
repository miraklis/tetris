#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image* loadImage(char* path)
{
    Image* image = (Image*)malloc(sizeof(Image));
    image->data = stbi_load(path, &image->width, &image->height, &image->channels, 0);
    if (!image->data) {
        const char* error = stbi_failure_reason();
        printf("Failed to load image: %s\n", error);
        //free(image);
        FREE(image);
        return NULL;
    }

    glGenTextures(1, &image->textureID);
    glBindTexture(GL_TEXTURE_2D, image->textureID);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Upload to GPU (ensure the format matches your loading; e.g., GL_RGB vs GL_RGBA)
    GLenum format = (image->channels == 4) ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    image->x = 0.0f;
    image->y = 0.0f;
    const float x = image->x;
    const float y = image->y;
    const float w = graphics.screenWidth;
    const float h = graphics.screenHeight;

    float vertices[] = {
        // Positions (x, y, z)      // Texture coords (u, v)
        w,      y,      0.0f,       1.0f,   1.0f,   // Top Right
        w,      h,      0.0f,       1.0f,   0.0f,   // Bottom Right
        x,      h,      0.0f,       0.0f,   0.0f,   // Bottom Left
        x,      y,      0.0f,       0.0f,   1.0f    // Top Left 
    };
    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 }; // Two triangles to form a quad
    
    glGenVertexArrays(1, &image->vao);
    glGenBuffers(1, &image->vbo);
    glGenBuffers(1, &image->ebo);
    glBindVertexArray(image->vao);
    glBindBuffer(GL_ARRAY_BUFFER, image->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, image->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    orthoMatrix(0, graphics.screenWidth, 0, graphics.screenHeight, -1, 1, image->proj);
    translateMatrix(image->x, image->y, image->model);
    image->visible = true;

    return image;
}

void moveImage(Image* img, float x, float y)
{
    img->x = x;
    img->y = y;
    translateMatrix(x, y, img->model);
}

void drawImage(Image* img, TextureShader* shader)
{
    if(img == NULL || !img->visible)
        return;
    
    useProgram(shader->program);
    glBindVertexArray(img->vao);
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, img->proj);
    glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, img->model);
    glBindTexture(GL_TEXTURE_2D, img->textureID); 
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shader->locTexture, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void destroyImage(Image* img)
{
    if(img==NULL)
        return;
    glDeleteVertexArrays(1, &img->vao);
    glDeleteBuffers(1, &img->vbo);
    glDeleteBuffers(1, &img->ebo);
    glDeleteTextures(1, &img->textureID);
    stbi_image_free(img->data);
    FREE(img);
}