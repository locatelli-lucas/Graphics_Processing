#include "Sprite.h"
#include "stb_image.h"
#include <iostream>

Sprite::Sprite(const std::string& imagePath) {
    loadTexture(imagePath);
}

Sprite::~Sprite() {
    glDeleteTextures(1, &textureID);
}

void Sprite::loadTexture(const std::string& imagePath) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Falha ao carregar textura: " << imagePath << std::endl;
    }
    stbi_image_free(data);
}

void Sprite::draw(float x, float y) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x, y);
    glTexCoord2f(1, 1); glVertex2f(x + 100, y);
    glTexCoord2f(1, 0); glVertex2f(x + 100, y + 100);
    glTexCoord2f(0, 0); glVertex2f(x, y + 100);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
