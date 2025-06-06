#pragma once
#include <glad/glad.h>

class Sprite {
    public:
        Sprite(const char* imagePath, float x, float y, float width, float height);
        ~Sprite();

        void draw();
        void setPosition(float newX, float newY);
        void setSize(float newWidth, float newHeight);
        void handleInput(GLFWwindow* window);

    private:
        float x, y;
        float width, height;
        int imgWidth, imgHeight;
        GLuint textureID;
};
