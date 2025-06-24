#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Sprite {
public:
    Sprite(const std::string& imagePath);
    ~Sprite();

    void draw(float x, float y);
    static void InitGLResources();
    static void SetProjection(const glm::mat4& proj);
private:
    GLuint textureID;
    int width, height, nrChannels;
    static GLuint shaderID;
    static GLuint VAO, VBO;
    static GLint modelLoc, projLoc, texLoc;
    void loadTexture(const std::string& imagePath);
};

#endif
