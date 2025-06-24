#include "Sprite.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint Sprite::shaderID = 0;
GLuint Sprite::VAO = 0;
GLuint Sprite::VBO = 0;
GLint Sprite::modelLoc = -1;
GLint Sprite::projLoc = -1;
GLint Sprite::texLoc = -1;

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Sprite::InitGLResources() {
    if (shaderID != 0) return;
    // Load and compile shaders
    std::string vsSource = readFile("src/M5/_sprites_vs.glsl");
    std::string fsSource = readFile("src/M5/_sprites_fs.glsl");
    const char* vs = vsSource.c_str();
    const char* fs = fsSource.c_str();
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vs, nullptr);
    glCompileShader(vShader);
    GLint success;
    char infoLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vShader, 512, nullptr, infoLog);
        std::cerr << "Erro ao compilar vertex shader:\n" << infoLog << std::endl;
    }
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fs, nullptr);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fShader, 512, nullptr, infoLog);
        std::cerr << "Erro ao compilar fragment shader:\n" << infoLog << std::endl;
    }
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vShader);
    glAttachShader(shaderID, fShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Erro ao linkar shader program:\n" << infoLog << std::endl;
    }
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    // Setup quad VAO/VBO em coordenadas 0 a 1, texcoords padrão
    float vertices[] = {
        // pos      // tex
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Uniform locations
    modelLoc = glGetUniformLocation(shaderID, "model");
    projLoc = glGetUniformLocation(shaderID, "projection");
    texLoc = glGetUniformLocation(shaderID, "sprite");
}

void Sprite::SetProjection(const glm::mat4& proj) {
    glUseProgram(shaderID);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
}

Sprite::Sprite(const std::string& imagePath) {
    loadTexture(imagePath);
}

Sprite::~Sprite() {
    glDeleteTextures(1, &textureID);
}

void Sprite::loadTexture(const std::string& imagePath) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        std::cerr << "Falha ao carregar textura: " << imagePath << std::endl;
    }
    stbi_image_free(data);
}

void Sprite::draw(float x, float y) {
    glUseProgram(shaderID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(texLoc, 0);
    glUniform1f(glGetUniformLocation(shaderID, "offsetx"), 0.0f);
    glUniform1f(glGetUniformLocation(shaderID, "fw"), 1.0f);
    // Forçar o quad a ocupar toda a tela para depuração
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(800.0f, 600.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    std::cerr << "[DEBUG] Antes do glDrawArrays, VAO=" << VAO << ", shaderID=" << shaderID << std::endl;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[ERRO OpenGL] glDrawArrays: " << err << std::endl;
    }
    glBindVertexArray(0);
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
