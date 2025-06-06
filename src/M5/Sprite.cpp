#include "Sprite.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>

Sprite::Sprite(const char* imagePath, float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height), textureID(0) {
    // Carrega a textura usando stb_image
    int nrChannels;
    unsigned char* data = stbi_load(imagePath, &imgWidth, &imgHeight, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Falha ao carregar imagem: " << imagePath << std::endl;
    }
}

Sprite::~Sprite() {
    if (textureID) {
        glDeleteTextures(1, &textureID);
    }
}

void Sprite::draw() {
    // Aqui você implementaria o desenho do sprite usando OpenGL
    // (bind da textura, configuração de vértices, etc.)
    // Este é um placeholder
    glBindTexture(GL_TEXTURE_2D, textureID);
    // ... código para desenhar um quad/textura ...
}

void Sprite::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Sprite::setSize(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;
}

void Sprite::handleInput(GLFWwindow* window) {
    float speed = 8.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        x -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        x += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        y += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        y -= speed;
    }
}
