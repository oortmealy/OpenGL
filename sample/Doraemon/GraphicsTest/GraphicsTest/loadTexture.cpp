#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "texture_loader.h"

GLuint faceTextureId = 0;


void loadFaceTexture(const char* filename) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        printf("이미지 로딩 실패: %s\n", filename);
        exit(1);
    }

    glGenTextures(1, &faceTextureId);
    glBindTexture(GL_TEXTURE_2D, faceTextureId);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
        format, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
}