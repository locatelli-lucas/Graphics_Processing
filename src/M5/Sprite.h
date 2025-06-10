#ifndef SPRITE_H
#define SPRITE_H

#include <GL/glew.h>
#include <string>

class Sprite {
public:
    Sprite(const std::string& imagePath);
    ~Sprite();

    void draw(float x, float y);
private:
    GLuint textureID;
    int width, height, nrChannels;

    void loadTexture(const std::string& imagePath);
};

#endif
